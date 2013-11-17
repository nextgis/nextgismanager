/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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

//------------------------------------------------------------------------------
// wxGISProgressDlg
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISProgressDlg, wxProgressDialog)

wxGISProgressDlg::wxGISProgressDlg( const wxString &title, const wxString &message, int  maximum, wxWindow *  parent, int style ) : wxProgressDialog(title, message, 100, parent, style), ITrackCancel()
{
	m_sLastMessage = message;
	m_nValue = 0;
	m_pProgressor = this;
    m_nRange = maximum;
    m_dfStep = 1;
    m_nPrevValue = wxNOT_FOUND;
    m_bAddPercentToMessage = false;
}

wxGISProgressDlg::~wxGISProgressDlg(void)
{
}

bool wxGISProgressDlg::ShowProgress(bool bShow)
{
	return wxProgressDialog::Show(bShow);
}

void wxGISProgressDlg::SetRange(int range)
{
    m_nRange = range;
    m_dfStep = range * 0.01; //1%
	//wxProgressDialog::SetRange(range);
}

int wxGISProgressDlg::GetRange(void) const
{
	return wxProgressDialog::GetRange();
}

int wxGISProgressDlg::GetValue(void) const
{
	return wxProgressDialog::GetValue();
}

void wxGISProgressDlg::Play(void)
{
    m_bIsCanceled = !wxProgressDialog::Pulse();// m_sLastMessage);
}

void wxGISProgressDlg::Stop(void)
{
    m_bIsCanceled = !wxProgressDialog::Update(wxNOT_FOUND);// , m_sLastMessage);
}

void wxGISProgressDlg::SetValue(int value)
{
	m_nValue = value;
    int nNewVal = float(value) / m_dfStep;

    if (m_nPrevValue == nNewVal)
        return;
    m_nPrevValue = nNewVal;

    if (nNewVal > 99 && !HasFlag(wxPD_AUTO_HIDE))
        return;
        
    if (m_bAddPercentToMessage)
    {
        m_bIsCanceled = !wxProgressDialog::Update(nNewVal, m_sLastMessage + wxString::Format(_(" - %d%% done"), nNewVal));
    }
    else
    {
        m_bIsCanceled = !wxProgressDialog::Update(nNewVal, m_sLastMessage);
    }
    Fit();
}

void wxGISProgressDlg::Cancel(void)
{
	m_bIsCanceled = true;
}

bool wxGISProgressDlg::Continue(void)
{
	return !wxProgressDialog::WasCancelled();
}

void wxGISProgressDlg::Reset(void)
{
	m_bIsCanceled = false;
	wxProgressDialog::Resume();
}

void wxGISProgressDlg::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
{
    if (eType == enumGISMessageErr || eType == enumGISMessageWarning)
    {
        MESSAGE msg = { eType, sMessage };
        m_saWarnings.push_back(msg);
    }

    if (sMessage.Len() > 255)
        m_sLastMessage = sMessage.Left(255) + wxT("...");
    else
        m_sLastMessage = sMessage;
	//m_bIsCanceled = !wxProgressDialog::Update(wxNOT_FOUND, m_sLastMessage);
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

