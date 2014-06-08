/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalogUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Dmitry Barishnikov
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

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxpending.h"
#include "wxgis/framework/progressdlg.h"

#include "../../art/mainframecat.xpm"
#include "../../art/process_working_16.xpm"
#include "../../art/process_working_48.xpm"

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxCatalogUI, wxGxCatalog);

wxGxCatalogUI::wxGxCatalogUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxCatalog(oParent, soName, soPath)
{
    m_oIcon = wxIcon(mainframecat_xpm);
}

wxGxCatalogUI::~wxGxCatalogUI(void)
{
}

void wxGxCatalogUI::EditProperties(wxWindow *parent)
{
    //show options
    IApplication* pApp = GetApplication();
    if(pApp)
        pApp->OnAppOptions();
}

wxIcon wxGxCatalogUI::GetLargeImage(void)
{
    return wxNullIcon;
}

wxIcon wxGxCatalogUI::GetSmallImage(void)
{
    return m_oIcon;
}

long wxGxCatalogUI::AddPending(long nParentId)
{
    wxGxObjectContainer *pGxObjectContainer = wxDynamicCast(GetRegisterObject(nParentId), wxGxObjectContainer);
    wxCHECK_MSG(pGxObjectContainer, wxNOT_FOUND, wxT("The parent GxObject is not exist or not a container"));
    //if not loaded load images to list
    if(m_oaPendingIconsSmall.empty())
    {
        wxImageList lst(16, 16);
        lst.Add(wxBitmap(process_working_16_xpm));
        for(size_t i = 0; i < lst.GetImageCount(); i++)
            m_oaPendingIconsSmall.push_back(lst.GetIcon(i));
    }
    if(m_oaPendingIconsLarge.empty())
    {
        wxImageList lst(48, 48);
        lst.Add(wxBitmap(process_working_48_xpm));
        for(size_t i = 0; i < lst.GetImageCount(); i++)
            m_oaPendingIconsLarge.push_back(lst.GetIcon(i));
    }

    wxGxPendingUI *pPend = new wxGxPendingUI(&m_oaPendingIconsSmall, &m_oaPendingIconsLarge, pGxObjectContainer);
    return pPend->GetId();
}

void wxGxCatalogUI::RemovePending(long nPendingId)
{
    if(nPendingId == wxNOT_FOUND)
        return;
    wxGxPendingUI *pPend = wxDynamicCast(GetRegisterObject(nPendingId), wxGxPendingUI);
    wxCHECK_RET(pPend, wxT("The Pending GxObject is not exist"));
    pPend->StopAndDestroy();
}

//------------------------------------------------------------------------------------------
// separate ops
//------------------------------------------------------------------------------------------
bool FolderDrop(const CPLString& pPath, const wxArrayString& GxObjectPaths, bool bMove)
{
    if(GxObjectPaths.GetCount() == 0)
        return false;

    //TODO: check input object type and output object type
    //if different types - export
    //dialog with wxGIS and choices for output types (raster and vector data)

    //create progress dialog
    wxString sOper(bMove == true ? _("Move") : _("Copy"));
    wxString sTitle = wxString::Format(_("%s %ld objects (files)"), sOper.c_str(), GxObjectPaths.GetCount());
    wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());

    wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), GxObjectPaths.GetCount(), pParentWnd);
    ProgressDlg.ShowProgress(true);
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    bool bCopyAsk = true;

    for(size_t i = 0; i < GxObjectPaths.GetCount(); ++i)
    {
		wxString sMessage = wxString::Format(_("%s %ld object (file) from %ld"), sOper.c_str(), i + 1, GxObjectPaths.GetCount());
//		ProgressDlg.SetTitle(sMessage);
		ProgressDlg.PutMessage(sMessage);
        if(!ProgressDlg.Continue())
            break;

        wxString sPath = GxObjectPaths[i];
        wxGxObject* pGxObj = pCatalog->FindGxObject(sPath);
        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObj);
        if(pGxObjectEdit)
        {
            if(bMove)
            {
                bool bShouldMove = true;
                wxGxObject* pParentGxObj = pGxObj->GetParent();
                if (pParentGxObj != NULL)
                {
                    bShouldMove = pParentGxObj->GetPath() != pPath;
                }
                if (bShouldMove && pGxObjectEdit->CanMove(pPath))
                {
                    if(!pGxObjectEdit->Move(pPath, &ProgressDlg))
                    {
                        wxMessageBox(wxString::Format(_("%s failed. Path: %s"), _("Move"), pGxObj->GetFullName()), _("Error"), wxOK | wxICON_ERROR);
                        return false;
                    }
                }
            }
            else if(pGxObjectEdit->CanCopy(pPath))
            {
                if (bMove && bCopyAsk)
                {
                    bCopyAsk = false;
                    if (wxMessageBox(wxString::Format(_("Cannot move path: %s.\nBut can copy. Proceed?"), pGxObj->GetFullName()), _("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxNO)
                        return false;

                }
                if(!pGxObjectEdit->Copy(pPath, &ProgressDlg))
                {
                    wxMessageBox(wxString::Format(_("%s failed. Path: %s"), _("Copy"), pGxObj->GetFullName()), _("Error"), wxOK | wxICON_ERROR);
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        ProgressDlg.SetValue(i);
    }

    return true;
}


/*
#include "wxgis/core/config.h"
#include "wxgis/core/globalfn.h"

wxGxCatalogUI::wxGxCatalogUI(bool bFast) : wxGxCatalog()
{
	m_pSelection = new wxGxSelection();

    m_bOpenLastPath = true;
    m_bHasInternal = false;

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);
    if(bFast)
        return;
    m_ImageListSmall.Add(wxBitmap(process_working_16_xpm));
    m_ImageListLarge.Add(wxBitmap(process_working_48_xpm));
}

wxGxCatalogUI::~wxGxCatalogUI()
{
}

void wxGxCatalogUI::Detach(void)
{
    if(m_bHasInternal)
    {
		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
		{
			wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
			if(pNode)
			{
				oConfig.DeleteNodeChildren(pNode);
				SerializePlugins(pNode, true);
			}
		}

		for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
			m_ObjectFactoriesArray[i]->PutCatalogRef(m_pExtCat);

	    wxDELETE(m_pSelection);
	    EmptyChildren();
    	EmptyDisabledChildren();
    }
    else
    {
		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
			oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/open_last_path")), m_bOpenLastPath);

	    wxDELETE(m_pSelection);
        wxGxCatalog::Detach();
    }
}

void wxGxCatalogUI::EditProperties(wxWindow *parent)
{
    //show options
    IApplication* pApp = GetApplication();
    if(pApp)
        pApp->OnAppOptions();
}

void wxGxCatalogUI::Init(IGxCatalog* pExtCat)
{
	if(m_bIsChildrenLoaded)
		return;

    m_pExtCat = pExtCat;

    if(pExtCat)
    {
        m_bHasInternal = true;
        GxObjectFactoryArray* poObjFactArr = pExtCat->GetObjectFactories();
        if(poObjFactArr)
        {
            for(size_t i = 0; i < poObjFactArr->size(); ++i)
            {
                m_ObjectFactoriesArray.push_back(poObjFactArr->at(i));
                poObjFactArr->at(i)->PutCatalogRef(this);
            }
        }


	    //loads current user and when local machine items

        //IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pExtCat);
        //GxObjectArray* pGxObjectArray = pGxObjectContainer->GetChildren();
        //if(pGxObjectArray)
        //    for(size_t i = 0; i < pGxObjectArray->size(); ++i)
        //        m_Children.push_back(pGxObjectArray->at(i));

	    LoadChildren();

	    m_bShowHidden = pExtCat->GetShowHidden();
	    m_bShowExt = pExtCat->GetShowExt();
    }
    else
    {
	    LoadObjectFactories();
	    LoadChildren();

		wxGISAppConfig oConfig = GetConfig();
		if(!oConfig.IsOk())
			return;

		m_bShowHidden = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), false);
	    m_bShowExt = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), true);
	    m_bOpenLastPath = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/open_last_path")), true);
    }
}

void wxGxCatalogUI::ObjectDeleted(long nObjectID)
{
    m_pSelection->RemoveDo(nObjectID);
    wxGxCatalog::ObjectDeleted(nObjectID);
}

IGxObject* wxGxCatalogUI::ConnectFolder(wxString sPath, bool bSelect)
{
    if(m_pGxDiscConnections)
    {
        IGxObject* pAddedObj = m_pGxDiscConnections->ConnectFolder(sPath);
        if(pAddedObj && bSelect)
        {
            m_pSelection->Select(pAddedObj->GetID(), false, IGxSelection::INIT_ALL);
            return pAddedObj;
        }
    }
    return NULL;
}

void wxGxCatalogUI::DisconnectFolder(CPLString sPath)
{
    if(m_pGxDiscConnections)
        m_pGxDiscConnections->DisconnectFolder(sPath);
}

void wxGxCatalogUI::SetLocation(wxString sPath)
{
    IGxObject* pObj = SearchChild(sPath);
	if(pObj)
		m_pSelection->Select(pObj->GetID(), false, IGxSelection::INIT_ALL);
	else
		ConnectFolder(sPath);
}

void wxGxCatalogUI::Undo(int nPos)
{
    if(m_pSelection->CanUndo())
    {
        long nID = m_pSelection->Undo(nPos);
        if(nID != wxNOT_FOUND && GxObjectMap[nID] != NULL)
		{
			m_pSelection->Select(nID, false, IGxSelection::INIT_ALL);
		}
    }
}

void wxGxCatalogUI::Redo(int nPos)
{
    if(m_pSelection->CanRedo())
    {
        long nID = m_pSelection->Redo(nPos);
        if(nID != wxNOT_FOUND && GxObjectMap[nID] != NULL)
		{
			m_pSelection->Select(nID, false, IGxSelection::INIT_ALL);
		}
    }
}

wxIcon wxGxCatalogUI::GetLargeImage(void)
{
    return wxNullIcon;
}

wxIcon wxGxCatalogUI::GetSmallImage(void)
{
    return wxIcon(mainframecat_xpm);
}
*/

