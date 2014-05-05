/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  toolbox classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
/*
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/geoprocessingui/gptooldlg.h"
#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/catalogui/gxcatalogui.h"
//#include "wxgis/framework/application.h"
#include "wxgis/core/globalfn.h"

#include <wx/stdpaths.h>

#include "../../art/toolbox_16.xpm"
#include "../../art/toolbox_48.xpm"
#include "../../art/tool_16.xpm"
#include "../../art/tool_48.xpm"
#include "../../art/toolboxfavor_16.xpm"
#include "../../art/toolboxfavor_48.xpm"
#include "../../art/toolexec_16.xpm"
#include "../../art/toolexec_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxToolbox
/////////////////////////////////////////////////////////////////////////
wxGxToolbox::wxGxToolbox() : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = NULL;
    m_pDataNode = NULL;
}

wxGxToolbox::wxGxToolbox(wxGxRootToolbox* pRootToolbox, wxXmlNode* pDataNode, wxIcon LargeToolboxIcon, wxIcon SmallToolboxIcon, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = pRootToolbox;
    m_pDataNode = pDataNode;
    if(m_pDataNode)
        m_sName = wxGetTranslation( m_pDataNode->GetAttribute(wxT("name"), NONAME) );
    m_LargeToolboxIcon = LargeToolboxIcon;
    m_SmallToolboxIcon = SmallToolboxIcon;
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxToolbox::~wxGxToolbox(void)
{
}

wxIcon wxGxToolbox::GetLargeImage(void)
{
	return m_LargeToolboxIcon;
}

wxIcon wxGxToolbox::GetSmallImage(void)
{
	return m_SmallToolboxIcon;
}

void wxGxToolbox::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxToolbox::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxToolbox::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

void wxGxToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    LoadChildrenFromXml(m_pDataNode);
    m_bIsChildrenLoaded = true;
}

wxString wxGxToolbox::GetName(void)
{
    return m_sName;
}

void wxGxToolbox::LoadChildrenFromXml(wxXmlNode* pNode)
{
    wxXmlNode *pChild = pNode->GetChildren();
    while (pChild)
    {
        if(pChild->GetName().IsSameAs(wxT("toolbox"), false))
        {
            wxGxToolbox* pToolbox = new wxGxToolbox(m_pRootToolbox, pChild, m_LargeToolboxIcon, m_SmallToolboxIcon, m_LargeToolIcon, m_SmallToolIcon);
            IGxObject* pGxObj = static_cast<IGxObject*>(pToolbox);
            if(!AddChild(pGxObj))
                wxDELETE(pGxObj);
        }
        else if(pChild->GetName().IsSameAs(wxT("tool"), false))
        {
            wxGxTool* pTool = new wxGxTool(m_pRootToolbox, pChild, m_LargeToolIcon, m_SmallToolIcon);
            IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
            if(!AddChild(pGxObj))
                wxDELETE(pGxObj);
        }
        pChild = pChild->GetNext();
    }
}

/////////////////////////////////////////////////////////////////////////
// wxGxRootToolbox
/////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxRootToolbox, wxObject)

wxGxRootToolbox::wxGxRootToolbox(void) : m_bIsChildrenLoaded(false)
{
    m_pToolMngr = NULL;
    m_LargeToolboxIcon = wxIcon(toolbox_48_xpm);
    m_SmallToolboxIcon = wxIcon(toolbox_16_xpm);
    m_LargeToolIcon = wxIcon(tool_48_xpm);
    m_SmallToolIcon = wxIcon(tool_16_xpm);
}

wxGxRootToolbox::~wxGxRootToolbox(void)
{
}

void wxGxRootToolbox::Init(wxXmlNode* const pConfigNode)
{
    LoadChildren();
}

void wxGxRootToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

    m_pRootToolbox = this;

    //Add task exec
    wxGxToolExecute* pToolExecute = new wxGxToolExecute(m_pRootToolbox);
    IGxObject* pGxObj = static_cast<IGxObject*>(pToolExecute);

    if( !AddChild(pGxObj) )
    {
        wxDELETE(pGxObj);
    }
    else
    {
        m_pToolMngr = static_cast<wxGISGPToolManager*>(pToolExecute);
    }

    //Add favorites
	bool bShowFavorites = oConfig.ReadBool(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/toolboxes/show_favorites")), true);
    if(bShowFavorites)
    {
        short nMax = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/toolboxes/max_favorites")), 10);
        wxGxFavoritesToolbox* pToolbox = new wxGxFavoritesToolbox(m_pRootToolbox, nMax, m_LargeToolIcon, m_SmallToolIcon);
        IGxObject* pGxObj = static_cast<IGxObject*>(pToolbox);
        if(!AddChild(pGxObj))
            wxDELETE(pGxObj);
    }
    wxXmlNode* pToolboxesChild = oConfig.GetConfigNode(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/toolboxes")));
    LoadChildrenFromXml(pToolboxesChild);

	m_bIsChildrenLoaded = true;
}

void wxGxRootToolbox::Serialize(wxXmlNode* pConfigNode)
{
}

wxGISGPToolManager* wxGxRootToolbox::GetGPToolManager(void)
{
    return m_pToolMngr;
}

IGPToolSPtr wxGxRootToolbox::GetGPTool(wxString sToolName)
{
    return m_pToolMngr->GetTool(sToolName, m_pCatalog);
}

long wxGxRootToolbox::Advise(wxEvtHandler* pEvtHandler)
{
	if(m_pToolMngr)
		return m_pToolMngr->Advise(pEvtHandler);
	return wxNOT_FOUND;
}
void wxGxRootToolbox::Unadvise(long nCookie)
{
	if(m_pToolMngr)
		m_pToolMngr->Unadvise(nCookie);
}

bool wxGxRootToolbox::PrepareTool(wxWindow* pParentWnd, IGPToolSPtr pTool, bool bSync)
{
    if(!pTool)
    {
        wxMessageBox(_("Wrong input tool!"), _("Error"), wxICON_ERROR | wxOK );
        return false;
    }
    //create tool config dialog
    wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(this, pTool, bSync, pParentWnd, wxID_ANY, pTool->GetDisplayName());
    pDlg->Show(true);

    return true;
}

void wxGxRootToolbox::ExecuteTool(wxWindow* pParentWnd, IGPToolSPtr pTool, bool bSync)
{
    if(bSync)
    {
        wxGxTaskExecDlg dlg(m_pToolMngr, pParentWnd, wxID_ANY, pTool->GetDisplayName());
        int nTaskID = m_pToolMngr->Execute(pTool, static_cast<ITrackCancel*>(&dlg));
        dlg.SetTaskID(nTaskID);
		long m_nCookie = m_pToolMngr->Advise(&dlg);
        dlg.ShowModal();
		m_pToolMngr->Unadvise(m_nCookie);
    }
    else
    {
        m_pToolMngr->Execute(pTool, NULL);
    }
}

/////////////////////////////////////////////////////////////////////////
// wxGxFavoritesToolbox
/////////////////////////////////////////////////////////////////////////

wxGxFavoritesToolbox::wxGxFavoritesToolbox(wxGxRootToolbox* pRootToolbox, short nMaxCount, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = pRootToolbox;
    m_nMaxCount = nMaxCount;
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxFavoritesToolbox::~wxGxFavoritesToolbox(void)
{
}

wxIcon wxGxFavoritesToolbox::GetLargeImage(void)
{
	return wxIcon(toolboxfavor_48_xpm);
}

wxIcon wxGxFavoritesToolbox::GetSmallImage(void)
{
	return wxIcon(toolboxfavor_16_xpm);
}

void wxGxFavoritesToolbox::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxFavoritesToolbox::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxFavoritesToolbox::DeleteChild(IGxObject* pChild)
{
	//bool bHasChildren = m_Children.size() > 0 ? true : false;
	//if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	//if(bHasChildren != m_Children.size() > 0 ? true : false)
	//	m_pCatalog->ObjectChanged(this);
	//return true;
}

void wxGxFavoritesToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    if(m_pRootToolbox)
    {
        wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
        if(pGPToolManager)
        {
			int nCount = std::min(pGPToolManager->GetToolCount(), m_nMaxCount);
            for(size_t i = 0; i < nCount; ++i)
            {
                wxGxTool* pTool = new wxGxTool(m_pRootToolbox, pGPToolManager->GetPopularTool(i), m_LargeToolIcon, m_SmallToolIcon);
                IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
                if(!AddChild(pGxObj))
                    wxDELETE(pGxObj);
           }
        }
    }

    m_bIsChildrenLoaded = true;
}

wxString wxGxFavoritesToolbox::GetName(void)
{
    return wxString(_("Recent"));
}

/////////////////////////////////////////////////////////////////////////
// wxGxToolExecute
/////////////////////////////////////////////////////////////////////////

wxGxToolExecute::wxGxToolExecute(wxGxRootToolbox* pRootToolbox) : wxGISGPToolManager()
{
    m_pRootToolbox = pRootToolbox;
    m_LargeToolIcon = wxIcon(toolexec_48_xpm);
    m_SmallToolIcon = wxIcon(toolexec_16_xpm);
}

wxGxToolExecute::~wxGxToolExecute(void)
{
}

wxIcon wxGxToolExecute::GetLargeImage(void)
{
	return m_LargeToolIcon;
}

wxIcon wxGxToolExecute::GetSmallImage(void)
{
	return m_SmallToolIcon;
}

void wxGxToolExecute::Refresh(void)
{
    m_pCatalog->ObjectRefreshed(GetID());
}

wxString wxGxToolExecute::GetName(void)
{
    if(m_nRunningTasks > 0)
        return wxString::Format(_("Executed list [%d]"), m_nRunningTasks);
    else
        return wxString(_("Executed list"));

}

int wxGxToolExecute::Execute(IGPToolSPtr pTool, ITrackCancel* pTrackCancel)
{
	int nTaskID = wxNOT_FOUND;
	if(pTrackCancel)
	{
		nTaskID = wxGISGPToolManager::Execute(pTool, pTrackCancel);
	}
	else
	{
		wxWindow* pWnd = dynamic_cast<wxWindow*>(GetApplication());
		wxGxTaskObject *pGxTaskObject = new wxGxTaskObject(this, pTool->GetDisplayName(), m_LargeToolIcon, m_SmallToolIcon);
		nTaskID = wxGISGPToolManager::Execute(pTool, static_cast<ITrackCancel*>(pGxTaskObject));
		pGxTaskObject->SetTaskID(nTaskID);
		//add gxobj
        IGxObject* pGxObject = static_cast<IGxObject*>(pGxTaskObject);
		if(!AddChild(pGxObject))
		{
			wxDELETE(pGxTaskObject);
			return wxNOT_FOUND;
		}
        else
        {
            if(m_pCatalog)
                m_pCatalog->ObjectAdded(pGxObject->GetID());
        }

		if(m_pCatalog)
			m_pCatalog->ObjectChanged(GetID());
	}
	return nTaskID;
}

void wxGxToolExecute::StartProcess(size_t nIndex)
{
    wxGISGPToolManager::StartProcess(nIndex);

    if(!m_pCatalog)
        return;
    for(size_t i = 0; i < m_Children.size(); ++i)
    {
        wxGxTaskObject* pGxTaskObject = dynamic_cast<wxGxTaskObject*>(m_Children[i]);
        if(pGxTaskObject && pGxTaskObject->GetTaskID() == nIndex && m_pCatalog)
            m_pCatalog->ObjectChanged(m_Children[i]->GetID());
    }
    m_pCatalog->ObjectChanged(GetID());
}

void wxGxToolExecute::OnFinish(IProcess* pProcess, bool bHasErrors)
{
    wxGISGPToolManager::OnFinish(pProcess, bHasErrors);
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pProcess == m_ProcessArray[nIndex].pProcess)
            break;

    if(!m_pCatalog)
        return;

    for(size_t i = 0; i < m_Children.size(); ++i)
    {
        wxGxTaskObject* pGxTaskObject = dynamic_cast<wxGxTaskObject*>(m_Children[i]);
        if(pGxTaskObject && pGxTaskObject->GetTaskID() == nIndex)
            m_pCatalog->ObjectChanged(m_Children[i]->GetID());
    }
    m_pCatalog->ObjectChanged(GetID());
}

bool wxGxToolExecute::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

/////////////////////////////////////////////////////////////////////////
// wxGxTool
/////////////////////////////////////////////////////////////////////////

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxXmlNode* pDataNode, wxIcon LargeToolIcon, wxIcon SmallToolIcon)
{
    m_pDataNode = pDataNode;
    m_pRootToolbox = pRootToolbox;
    if(m_pDataNode && m_pRootToolbox)
        m_sInternalName = m_pDataNode->GetAttribute(wxT("name"), NONAME);

    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxString sInternalName, wxIcon LargeToolIcon, wxIcon SmallToolIcon)
{
    m_pDataNode = NULL;
    m_pRootToolbox = pRootToolbox;
    m_sInternalName = sInternalName;

    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxTool::~wxGxTool(void)
{
}

wxIcon wxGxTool::GetLargeImage(void)
{
	return m_LargeToolIcon;
}

wxIcon wxGxTool::GetSmallImage(void)
{
	return m_SmallToolIcon;
}

bool wxGxTool::Invoke(wxWindow* pParentWnd)
{
    //callback create/destroy gxtask
	IGPToolSPtr pTool = m_pRootToolbox->GetGPTool(m_sInternalName);
	if(!pTool)
	{
        //error msg
		wxMessageBox(wxString::Format(_("Error find %s tool!\nCannnot continue."), m_sInternalName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return false; 
	}

	m_pRootToolbox->PrepareTool(pParentWnd, pTool, false);
    return true;
}

bool wxGxTool::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
    if(!IGxObject::Attach(pParent, pCatalog))
        return false;
    wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
    IGPToolSPtr pTool = pGPToolManager->GetTool(m_sInternalName, m_pCatalog);
    if(pTool)
    {
        m_sName = pTool->GetDisplayName();
        return true;
    }
    else
        return false;
}
*/