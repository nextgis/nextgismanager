/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxAxToolboxView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Dmitry Barishnikov
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

#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/framework/droptarget.h"

//-------------------------------------------------------------------
// wxAxToolboxView
//-------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxAxToolboxView, wxAuiNotebook)

wxAxToolboxView::wxAxToolboxView(void)
{
}

wxAxToolboxView::wxAxToolboxView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxAuiNotebook(parent, id, pos, size, wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE)
{
    Create(parent, id, pos, size);
}

wxAxToolboxView::~wxAxToolboxView(void)
{
}

bool wxAxToolboxView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("Toolbox"));
    bool bRes = wxAuiNotebook::Create(parent, id, pos, size, wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE);
#ifdef __WXGTK__
	SetArtProvider(new wxGISTabArt());
#endif // __WXGTK__
    return bRes;
}

bool wxAxToolboxView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if (!wxGxView::Activate(pApplication, pConf))
		return false;

	m_pApp = wxDynamicCast(pApplication, wxGISApplicationBase);
	if (!m_pApp)
	{
		return false;
	}

    //get config xml
    wxXmlNode *pTaskExecConf(NULL), *pToolboxTreeConf(NULL);
    wxXmlNode* pChildConf = pConf->GetChildren();
    while(pChildConf)
    {
        if(pChildConf->GetName() == wxString(wxT("wxGxToolExecuteView")))
            pTaskExecConf = pChildConf;
        if(pChildConf->GetName() == wxString(wxT("wxGxToolboxTreeView")))
            pToolboxTreeConf = pChildConf;

        pChildConf = pChildConf->GetNext();
    }
    if(!pTaskExecConf)
    {
        pTaskExecConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolExecuteView")));
        pConf->AddChild(pTaskExecConf);
    }
    if(!pToolboxTreeConf)
    {
        pToolboxTreeConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolboxTreeView")));
        pConf->AddChild(pToolboxTreeConf);
    }

    int nOSMajorVer(0);
    wxGetOsVersion(&nOSMajorVer);
    m_pGxToolboxView = new wxGxToolboxTreeView(this, TREECTRLID, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS | (nOSMajorVer > 5 ? wxTR_NO_LINES : 0));
    AddPage(m_pGxToolboxView, m_pGxToolboxView->GetViewName(), true, m_pGxToolboxView->GetViewIcon());
    m_pGxToolboxView->Activate(pApplication, pToolboxTreeConf);
    m_pApp->RegisterChildWindow(m_pGxToolboxView->GetId());

	/*
    wxGxToolExecute* pGxToolExecute(NULL);
    IGxObjectContainer* pRootContainer = dynamic_cast<IGxObjectContainer*>(m_pGxApplication->GetCatalog());
    if(pRootContainer)
    {
        IGxObjectContainer* pGxToolboxes = dynamic_cast<IGxObjectContainer*>(pRootContainer->SearchChild(wxString(_("Toolboxes"))));
        if(pGxToolboxes)
        {
            GxObjectArray* pArr = pGxToolboxes->GetChildren();
            if(pArr)
            {
                for(size_t i = 0; i < pArr->size(); ++i)
                {
                    pGxToolExecute = dynamic_cast<wxGxToolExecute*>(pArr->operator[](i));
                    if(pGxToolExecute)
                        break;
                }
            }
        }
    }

    if(pGxToolExecute)
    {
        m_pGxToolExecuteView = new wxGxToolExecuteView(this, TOOLEXECUTECTRLID);
        AddPage(m_pGxToolExecuteView, m_pGxToolExecuteView->GetViewName(), false, m_pGxToolExecuteView->GetViewIcon());
        m_pGxToolExecuteView->Activate(application, pTaskExecConf);
        m_pGxToolExecuteView->SetGxToolExecute(pGxToolExecute);
        m_pApp->RegisterChildWindow(m_pGxToolExecuteView);
    }
	*/
	return true;
}

void wxAxToolboxView::Deactivate(void)
{
	/*
    m_pGxToolExecuteView->Deactivate();
    m_pApp->UnRegisterChildWindow(m_pGxToolExecuteView);
	*/
	if (m_pGxToolboxView)
	{
		m_pGxToolboxView->Deactivate();
		m_pApp->UnRegisterChildWindow(m_pGxToolboxView->GetId());
	}
}

//-------------------------------------------------------------------
// wxGxToolsExecView
//-------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxToolboxTreeView, wxGxTreeView)

wxGxToolboxTreeView::wxGxToolboxTreeView(void) : wxGxTreeView()
{
}

wxGxToolboxTreeView::wxGxToolboxTreeView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeView(parent, id, style)
{
	SetDropTarget(new wxGISDropTarget(static_cast<IViewDropTarget*>(this)));
    m_sViewName = wxString(_("Toolboxes"));
}

wxGxToolboxTreeView::~wxGxToolboxTreeView(void)
{
}

bool wxGxToolboxTreeView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if (!wxGxView::Activate(pApplication, pConf))
		return false;

	m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
	m_pGxApp = dynamic_cast<wxGxApplication*>(pApplication);
	if (NULL == m_pGxApp)
	{
		return false;
	}
	m_pSelection = m_pGxApp->GetGxSelection();
	//m_pConnectionPointSelection = dynamic_cast<wxGISConnectionPointContainer*>( m_pSelection );
	//if(m_pConnectionPointSelection != NULL)
	//	m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	//delete
	m_pDeleteCmd = m_pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);

	if (NULL == GetGxCatalog())
		return false;
	m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

	//TODO: wxDynamicCast(pGxCatalog->GetRootItemByType(wxCLASSINFO(wxGxDiscConnections)), wxGxDiscConnections);
	wxGxObject* pGxToolboxes = m_pCatalog->FindGxObject(wxString(_("Toolboxes")));
	AddRoot(pGxToolboxes);

	m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);

	return true;
}

void wxGxToolboxTreeView::UpdateGxSelection(void)
{
	wxTreeItemId TreeItemId = GetSelection();
	m_pSelection->Clear(GetId());
	//if(TreeItemId.IsOk())
    //{
    //    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
    //    if(pData != NULL)
	   //     m_pSelection->Select(pData->m_pObject, true, GetId());
    //}
	if (m_pGxApp)
	{
        m_pGxApp->UpdateNewMenu(m_pSelection);
	}
}


