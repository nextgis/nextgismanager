/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "wxgis/framework/progressdlg.h"
#include "wxgis/framework/progressor.h"

#include <wx/statline.h>

//------------------------------------------------------------------------------
// wxGISProgressDlg
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISProgressDlg, wxDialog)

BEGIN_EVENT_TABLE(wxGISProgressDlg, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxGISProgressDlg::OnCancel)
END_EVENT_TABLE()

wxGISProgressDlg::wxGISProgressDlg(const wxString &title, const wxString &message, int  maximum, wxWindow *  parent, int style) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, style), ITrackCancel()

{
	m_sLastMessage = message;
    m_bAddPercentToMessage = false;

    m_pProgressor = this;

    wxBoxSizer* bMainSizer = new wxBoxSizer(wxVERTICAL);

    m_staticText = new wxStaticText(this, wxID_ANY, m_sLastMessage, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_MIDDLE);
    m_staticText->SetLabel(wxT("                                                              \n                                                              "));
    //m_staticText->Wrap(-1);
    bMainSizer->Add(m_staticText, 1, wxALL | wxEXPAND, 5);

    wxGISProgressor* pProgressBar = new wxGISProgressor(this, wxID_ANY);
    //pProgressBar->SetYield(true);
    bMainSizer->Add(pProgressBar, 0, wxALL | wxEXPAND, 5);
    m_pProgressBar = static_cast<IProgressor*>(pProgressBar);
    m_pProgressBar->SetRange(maximum);

    m_staticElapsedText = new wxStaticText(this, wxID_ANY, _("Remaining time:") + wxT(" 00:00:00"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticElapsedText->Wrap(-1);
    bMainSizer->Add(m_staticElapsedText, 0, wxALL | wxEXPAND, 5);

    wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);

    m_sdbSizer = new wxStdDialogButtonSizer();
    //m_sdbSizer1OK = new wxButton( this, wxID_OK );
    //m_sdbSizer1->AddButton( m_sdbSizer1OK );
    m_sdbSizerCancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    m_sdbSizer->AddButton(m_sdbSizerCancel);
    m_sdbSizer->Realize();
    bMainSizer->Add(m_sdbSizer, 0, wxEXPAND | wxALL, 5);

    this->SetSizerAndFit(bMainSizer);
    this->Layout();

    this->Centre(wxBOTH);

    m_dtStart = wxDateTime::Now();
#ifdef __WXMSW__
    m_pTaskbarList = NULL;
    CoCreateInstance(
        CLSID_TaskbarList,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pTaskbarList));
#endif
}

wxGISProgressDlg::~wxGISProgressDlg(void)
{
#ifdef __WXMSW__
    wxFrame *pMainFrame = dynamic_cast<wxFrame *>(GetApplication());
    if (pMainFrame && m_pTaskbarList)
        m_pTaskbarList->SetProgressState((HWND)pMainFrame->GetHandle(), TBPF_NOPROGRESS);
#endif

}

bool wxGISProgressDlg::ShowProgress(bool bShow)
{
	bool bRet = wxDialog::Show(bShow);
    Update();

    return bRet;
}

void wxGISProgressDlg::SetRange(int range)
{
    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetRange(range);
        m_dtStart = wxDateTime::Now();
        SetValue(0);
    }

#ifdef __WXMSW__
    wxFrame *pMainFrame = dynamic_cast<wxFrame *>(GetApplication());
    if (pMainFrame && m_pTaskbarList)
        m_pTaskbarList->SetProgressState((HWND)pMainFrame->GetHandle(), TBPF_NORMAL);
#endif

}

int wxGISProgressDlg::GetRange(void) const
{
    if (NULL != m_pProgressBar)
    {
        return m_pProgressBar->GetRange();
    }
    return 1;
}

int wxGISProgressDlg::GetValue(void) const
{
    if (NULL != m_pProgressBar)
    {
        return m_pProgressBar->GetValue();
    }
    return 0;
}

void wxGISProgressDlg::Play(void)
{
    if (NULL != m_pProgressBar)
    {
        return m_pProgressBar->Play();
    }

#ifdef __WXMSW__
    wxFrame *pMainFrame = dynamic_cast<wxFrame *>(GetApplication());
    if (pMainFrame && m_pTaskbarList)
        m_pTaskbarList->SetProgressState((HWND)pMainFrame->GetHandle(), TBPF_INDETERMINATE);
#endif

}

void wxGISProgressDlg::Stop(void)
{
    if (NULL != m_pProgressBar)
    {
        return m_pProgressBar->Stop();
    }
}

void wxGISProgressDlg::SetValue(int value)
{
    if (NULL != m_pProgressBar)
    {
        m_pProgressBar->SetValue(value);
    }

    wxTheApp->Yield(true);

#ifdef __WXMSW__
    wxFrame *pMainFrame = dynamic_cast<wxFrame *>(GetApplication());
    if (pMainFrame && m_pTaskbarList)
        m_pTaskbarList->SetProgressValue((HWND)pMainFrame->GetHandle(), value, GetRange());
#endif

    wxTimeSpan Elapsed = wxDateTime::Now() - m_dtStart;
    int nRange = GetRange();
    if (nRange < 1)
        nRange = 1;
    double dfDone = double(value) / nRange;
    double dfToDo = 1.0 - dfDone;
    int nDone = dfDone * 100;
    if (m_nPrevDone == nDone)
        return;
    m_nPrevDone = nDone;
    if (nDone == 0)
    {
        m_staticElapsedText->SetLabel(_("Remaining time:") + wxT(" 00:00:00"));
        return;
    }

    if (nDone % 1 != 0)
        return;

    long dMSec = double(Elapsed.GetMilliseconds().ToDouble() * dfToDo) / dfDone;
    wxTimeSpan Remains = wxTimeSpan(0, 0, 0, dMSec);

    if (NULL != m_staticElapsedText)
    {
        m_staticElapsedText->SetLabel(_("Remaining time:") + wxT(" ") + Remains.Format());
    }

}

void wxGISProgressDlg::Cancel(void)
{
	m_bIsCanceled = true;
}


void wxGISProgressDlg::OnCancel(wxCommandEvent& event)
{
    Cancel();
}

bool wxGISProgressDlg::Continue(void)
{
	return !m_bIsCanceled;
}

void wxGISProgressDlg::Reset(void)
{
	m_bIsCanceled = false;
}

void wxGISProgressDlg::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
{
    wxString sMsg;
    if (eType == enumGISMessageErr || eType == enumGISMessageWarning)
    {
        sMsg = sMessage.Len() < 600 ? sMessage : sMessage.Left(597) + wxT("...");
        sMsg.Replace(wxT("%"), wxEmptyString);
        MESSAGE msg = { eType, sMsg};
        m_saWarnings.push_back(msg);
    }
    else
    {
        sMsg = sMessage;
        sMsg.Replace(wxT("%"), wxEmptyString);
    }

    wxString sNewLastMessage;
    if (sMsg.Len() > 255)
        sNewLastMessage = sMsg.Left(255) + wxT("...");
    else
        sNewLastMessage = sMsg;

    m_sLastMessage = sNewLastMessage;

    if (NULL != m_staticText)
    {
        if (m_bAddPercentToMessage)
        {
            int nRange = GetRange();
            if (nRange < 1)
                nRange = 1;
            int nDone = double(GetValue()) / nRange * 100;

            m_staticText->SetLabel(m_sLastMessage + wxString::Format(_(" - %d%% done"), nDone));
        }
        else
        {
            m_staticText->SetLabel(m_sLastMessage);
        }
    }

    //Fit();

    wxTheApp->Yield(true);
}

void wxGISProgressDlg::SetAddPercentToMessage(bool bAdd)
{
    m_bAddPercentToMessage = bAdd;
}

size_t wxGISProgressDlg::GetWarningCount() const
{
    return m_saWarnings.size();
}

const wxVector<MESSAGE>& wxGISProgressDlg::GetWarnings() const
{
    return m_saWarnings;
}

void wxGISProgressDlg::SetYield(bool bYield)
{
    if (NULL != m_pProgressBar)
    {
        return m_pProgressBar->SetYield(bYield);
    }
}

