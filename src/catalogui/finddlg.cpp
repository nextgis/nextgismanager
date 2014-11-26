/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPropertyDlg class - dialog/dock window with the GxObject properties.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalogui/finddlg.h"

/*
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/framework/tabstyle.h"
#include "wxgis/catalog/gxcatalog.h"

#include <wx/statline.h>

//-------------------------------------------------------------------
// IGxPropertyPage
//-------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(wxGxPropertyPage, wxPanel)

wxGxPropertyPage::wxGxPropertyPage()
{
	m_bCanMerge = false;
	m_PageIcon = wxNullBitmap;	
	m_pTrackCancel = NULL;
}
 
wxGxPropertyPage::~wxGxPropertyPage(void)
{
	
}

bool wxGxPropertyPage::CanMerge() const
{
	return m_bCanMerge;
}

wxString wxGxPropertyPage::GetPageName(void) const
{
	return m_sPageName;
}

wxBitmap wxGxPropertyPage::GetIcon() const
{
	return m_PageIcon;
}

//-------------------------------------------------------------------
// wxGISPropertyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISPropertyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISPropertyDlg, wxPanel)
    EVT_BUTTON(wxID_APPLY, wxGISPropertyDlg::OnApply)
	EVT_UPDATE_UI(wxID_APPLY, wxGISPropertyDlg::OnApplyUI)
END_EVENT_TABLE()


wxGISPropertyDlg::wxGISPropertyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : ITrackCancel()
{
	Create(parent, id, pos, size, style);
}

wxGISPropertyDlg::wxGISPropertyDlg(void) : ITrackCancel()
{
}

bool wxGISPropertyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;
		


	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
    }

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTabs	= new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE);
#ifdef __WXGTK__
	m_pTabs->SetArtProvider(new wxGISTabArt());
#endif // __WXGTK__		
	m_bMainSizer->Add(m_pTabs, 1, wxALL | wxEXPAND, 5);
	wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    m_bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);
	
	wxStdDialogButtonSizer* sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerApply = new wxButton(this, wxID_APPLY, _("Apply"));
	sdbSizer->AddButton(m_sdbSizerApply);
    sdbSizer->Realize();
    m_bMainSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, 5);

	this->SetSizer( m_bMainSizer );
	this->Layout();

    return true;
}

wxGISPropertyDlg::~wxGISPropertyDlg()
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
    }
}

void wxGISPropertyDlg::Update(wxGxSelection* const pSel)
{
	wxGxCatalogBase* pCat = GetGxCatalog();	
	if(NULL == pSel || NULL == pCat)
		return;		
		
	wxArrayString saAllPages;
	
//1. Fill all property pages array	
	for ( size_t i = 0; i < pSel->GetCount(); ++i ) 
	{    
		long nID = pSel->GetSelectedObjectId(i);
		wxGxObject* pObj = pCat->GetRegisterObject(nID);
		IGxObjectEditUI* pGxObjectEdit = dynamic_cast<IGxObjectEditUI*>(pObj);
		if(pGxObjectEdit)
		{
			wxArrayString saPages = pGxObjectEdit->GetPropertyPages();
			for ( size_t j = 0; j < saPages.GetCount(); ++j ) 
			{    
				if(saAllPages.Index(saPages[j]) == wxNOT_FOUND)	
				{
					saAllPages.Add(saPages[j]);
				}
			}			
		}
	}
	
	if(saAllPages.IsEmpty())
	{
		m_pTabs->DeleteAllPages();
	}
	
//2. Check each property page is exist in dialog and can merge
	for ( size_t j = 0; j < m_pTabs->GetPageCount(); ++j ) 
	{    
		wxWindow* pPage = m_pTabs->GetPage(j);
		if(pPage)
		{
			int nPos = wxNOT_FOUND;
			if((nPos = saAllPages.Index(pPage->GetClassInfo()->GetClassName())) != wxNOT_FOUND)
			{
				saAllPages.RemoveAt(nPos);
				wxGxPropertyPage *pPPage = dynamic_cast<wxGxPropertyPage*>(pPage);
				if(pPPage)
				{
					if(pSel->GetCount() > 1 && !pPPage->CanMerge())
					{
						m_pTabs->DeletePage(j--);
					}
				}
			}
			else
			{
				//remove page
				m_pTabs->DeletePage(j--);
			}
		}
	}
	
//3. Instantinate new PropertyPages and check thea can merge if can - add to dialog
	for ( size_t i = 0; i < saAllPages.GetCount(); ++i ) 
	{    
		wxClassInfo* pClassInfo = wxClassInfo::FindClass(saAllPages[i]);
		if(pClassInfo)
		{
			wxObject* pObj = pClassInfo->CreateObject();
			if(pObj)
			{
				wxGxPropertyPage *pPPage = dynamic_cast<wxGxPropertyPage*>(pObj);
				if(pPPage)
				{
					if((pSel->GetCount() == 1 || pPPage->CanMerge()) && pPPage->Create(this, m_pTabs))
					{
						m_pTabs->AddPage(static_cast<wxWindow*>(pPPage), pPPage->GetPageName(), false, pPPage->GetIcon());
					}
				}
			}
		}
	}

//4. Init all pages with input GxObjects
	for ( size_t i = 0; i < m_pTabs->GetPageCount(); ++i ) 
	{    
		wxGxPropertyPage *pPPage = dynamic_cast<wxGxPropertyPage*>(m_pTabs->GetPage(i));
		if(pPPage)
		{
			pPPage->FillProperties(pSel);
		}
	}
 }
 
void wxGISPropertyDlg::OnApply(wxCommandEvent& event)
{
	if(NULL != m_pTabs)
	{
		for ( size_t i = 0; i < m_pTabs->GetPageCount(); ++i ) 
		{    
			wxGxPropertyPage* pPage = wxDynamicCast(m_pTabs->GetPage(i), wxGxPropertyPage);
			if(pPage)
			{ 
				pPage->Apply();
			}
		}	
	}	
}

void wxGISPropertyDlg::OnApplyUI(wxUpdateUIEvent& event)
{
	event.Enable(false);
	if(NULL != m_pTabs)
	{
		for ( size_t i = 0; i < m_pTabs->GetPageCount(); ++i ) 
		{    
			wxGxPropertyPage* pPage = wxDynamicCast(m_pTabs->GetPage(i), wxGxPropertyPage);
			if(pPage && pPage->CanApply())
			{
				event.Enable(true);
				return;
			}
		}	
	}
}

//-------------------------------------------------------------------
// wxAxPropertyView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxPropertyView, wxGISPropertyDlg)

BEGIN_EVENT_TABLE(wxAxPropertyView, wxGISPropertyDlg)
	EVT_GXSELECTION_CHANGED(wxAxPropertyView::OnSelectionChanged)
END_EVENT_TABLE()

wxAxPropertyView::wxAxPropertyView(void)
{
	m_ConnectionPointSelectionCookie = wxNOT_FOUND;
	m_pSelection = NULL;
}

wxAxPropertyView::wxAxPropertyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISPropertyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
	m_ConnectionPointSelectionCookie = wxNOT_FOUND;
	m_pSelection = NULL;
    Create(parent, id, pos, size);
}

wxAxPropertyView::~wxAxPropertyView(void)
{
}

bool wxAxPropertyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("Properties"));
    return wxGISPropertyDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxPropertyView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{	
    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;
		
	wxGxApplicationBase* pGxApp = dynamic_cast<wxGxApplicationBase*>(pApplication);
    if(NULL == pGxApp)
        return false;
    m_pSelection = pGxApp->GetGxSelection();

	if (NULL != m_pSelection)
	{
        m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);
	}		

	return true;
}

void wxAxPropertyView::Deactivate(void)
{
	if (m_ConnectionPointSelectionCookie != wxNOT_FOUND && NULL != m_pSelection)
	{
        m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);
	}	
}

void wxAxPropertyView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(m_pApp->IsApplicationWindowShown(this))
		Update(event.GetSelection());
}

IProgressor* const wxAxPropertyView::GetProgressor(void)
{
	if(m_pApp)
	{
		wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
		if(pStatusBar)
			return pStatusBar->GetProgressor();
	}
	return NULL;
}

void wxAxPropertyView::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
{
	if(m_pApp)
	{
		wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
		if(pStatusBar)
			pStatusBar->SetMessage(sMessage);
	}	
}
*/