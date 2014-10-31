/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/catalogcmd.h"

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalog/gxdiscconnections.h"
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"
#include "wxgis/catalogui/gxlocationcombobox.h"
#include "wxgis/net/mail/email.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/processing.h"

//
//#include "wxgis/framework/progressor.h"

#include "../../art/delete.xpm"
#include "../../art/edit.xpm"

#include "../../art/folder_conn_new.xpm"
#include "../../art/folder_conn_del.xpm"
#include "../../art/folder_new.xpm"
#include "../../art/folder_up.xpm"

#include "../../art/view-refresh.xpm"
#include "../../art/go.xpm"
#include "../../art/properties.xpm"

#include "../../art/edit_copy.xpm"
#include "../../art/edit_cut.xpm"
#include "../../art/edit_paste.xpm"

#include "../../art/email.xpm"
#include "../../art/connect.xpm"

#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/richmsgdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/mimetype.h>
#include <wx/uri.h>

//-----------------------------------------------------------------------------------
// wxGISCatalogMainCmd
//-----------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogMainCmd, wxGISCommand)

wxGISCatalogMainCmd::wxGISCatalogMainCmd(void) : wxGISCommand()
{
}

wxGISCatalogMainCmd::~wxGISCatalogMainCmd(void)
{
}

wxIcon wxGISCatalogMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
			if(!m_IconFolderUp.IsOk())
				m_IconFolderUp = wxIcon(folder_up_xpm);
			return m_IconFolderUp;
        case enumGISCatalogMainCmdConnectCurrentFolder:
		case enumGISCatalogMainCmdConnectFolder:
			if(!m_IconFolderConn.IsOk())
				m_IconFolderConn = wxIcon(folder_conn_new_xpm);
			return m_IconFolderConn;
		case enumGISCatalogMainCmdDisconnectFolder:
			if(!m_IconFolderConnDel.IsOk())
				m_IconFolderConnDel = wxIcon(folder_conn_del_xpm);
			return m_IconFolderConnDel;
		case enumGISCatalogMainCmdDelete:
			if(!m_IconDel.IsOk())
				m_IconDel = wxIcon(delete_xpm);
			return m_IconDel;
		case enumGISCatalogMainCmdBack:
			if(!m_IconGoPrev.IsOk())
			{
				wxBitmap oGoOrigin(go_xpm);
				wxImage oGoPrevious = oGoOrigin.ConvertToImage();
				oGoPrevious = oGoPrevious.Mirror(true);

				m_IconGoPrev.CopyFromBitmap(wxBitmap(oGoPrevious));
			}
			return m_IconGoPrev;
		case enumGISCatalogMainCmdForward:
			if(!m_IconGoNext.IsOk())
				m_IconGoNext = wxIcon(go_xpm);
			return m_IconGoNext;
		case enumGISCatalogMainCmdCreateFolder:
			if(!m_IconFolderNew.IsOk())
				m_IconFolderNew = wxIcon(folder_new_xpm);
			return m_IconFolderNew;
		case enumGISCatalogMainCmdRename:
			if(!m_IconEdit.IsOk())
				m_IconEdit = wxIcon(edit_xpm);
			return m_IconEdit;
		case enumGISCatalogMainCmdRefresh:
			if(!m_IconViewRefresh.IsOk())
				m_IconViewRefresh = wxIcon(view_refresh_xpm);
			return m_IconViewRefresh;
		case enumGISCatalogMainCmdProperties:
			if(!m_IconProps.IsOk())
				m_IconProps = wxIcon(properties_xpm);
			return m_IconProps;
		case enumGISCatalogMainCmdCopy:
			if(!m_CopyIcon.IsOk())
				m_CopyIcon = wxIcon(edit_copy_xpm);
			return m_CopyIcon;
		case enumGISCatalogMainCmdCut:
			if(!m_CutIcon.IsOk())
				m_CutIcon = wxIcon(edit_cut_xpm);
			return m_CutIcon;
		case enumGISCatalogMainCmdPaste:
			if(!m_PasteIcon.IsOk())
				m_PasteIcon = wxIcon(edit_paste_xpm);
			return m_PasteIcon;
		case enumGISCatalogMainCmdSendEmail:
            if (!m_EmailIcon.IsOk())
                m_EmailIcon = wxIcon(email_xpm);
            return m_EmailIcon;
		case enumGISCatalogMainCmdConnect:	
		case enumGISCatalogMainCmdDisconnect:
			if(!m_ConnectIcon.IsOk())
				m_ConnectIcon = wxIcon(connect_xpm);
			return m_ConnectIcon;	
		case enumGISCatalogMainCmdLinkToClipboard:
		case enumGISCatalogMainCmdLocation:
		default:
			return wxNullIcon;
	}
}

wxString wxGISCatalogMainCmd::GetCaption(void)
{
    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
			return wxString(_("&Up One Level"));
		case enumGISCatalogMainCmdConnectFolder:
			return wxString(_("&Connect folder"));
        case enumGISCatalogMainCmdConnectCurrentFolder:
			return wxString(_("Co&nnect current folder"));
		case enumGISCatalogMainCmdDisconnectFolder:
			return wxString(_("&Disconnect folder"));
		case enumGISCatalogMainCmdLocation:
			return wxString(_("Location"));
		case enumGISCatalogMainCmdDelete:
			return wxString(_("Delete"));
		case enumGISCatalogMainCmdBack:
			return wxString(_("Back"));
		case enumGISCatalogMainCmdForward:
			return wxString(_("Forward"));
		case enumGISCatalogMainCmdCreateFolder:
			return wxString(_("Create folder"));
		case enumGISCatalogMainCmdRename:
			return wxString(_("Rename"));
		case enumGISCatalogMainCmdRefresh:
			return wxString(_("Refresh"));
		case enumGISCatalogMainCmdProperties:
			return wxString(_("Properties"));
		case enumGISCatalogMainCmdCopy:
			return wxString(_("Copy"));
		case enumGISCatalogMainCmdCut:
			return wxString(_("Cut"));
		case enumGISCatalogMainCmdPaste:
			return wxString(_("Paste"));
		case enumGISCatalogMainCmdSendEmail:
			return wxString(_("e-mail to..."));
		case enumGISCatalogMainCmdConnect:	
			return wxString(_("Connect"));
		case enumGISCatalogMainCmdDisconnect:	
			return wxString(_("Disconnect"));			
		case enumGISCatalogMainCmdLinkToClipboard:	
			return wxString(_("Link to clipboard"));			
		default:
			return wxEmptyString;
	}
}

wxString wxGISCatalogMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
		case enumGISCatalogMainCmdConnectFolder:
        case enumGISCatalogMainCmdConnectCurrentFolder:
		case enumGISCatalogMainCmdDisconnectFolder:
		case enumGISCatalogMainCmdLocation:
		case enumGISCatalogMainCmdBack:
		case enumGISCatalogMainCmdForward:
		case enumGISCatalogMainCmdRefresh:
			return wxString(_("Catalog"));
		case enumGISCatalogMainCmdCreateFolder:
			return wxString(_("Create"));
		case enumGISCatalogMainCmdDelete:
		case enumGISCatalogMainCmdRename:
		case enumGISCatalogMainCmdCopy:
		case enumGISCatalogMainCmdCut:
		case enumGISCatalogMainCmdPaste:
			return wxString(_("Edit"));
		case enumGISCatalogMainCmdProperties:
			return wxString(_("Miscellaneous"));
		case enumGISCatalogMainCmdSendEmail:
		case enumGISCatalogMainCmdLinkToClipboard:
			return wxString(_("Send to"));
		case enumGISCatalogMainCmdConnect:
		case enumGISCatalogMainCmdDisconnect:
			return wxString(_("Remote service"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISCatalogMainCmd::GetChecked(void)
{
	return false;
}

bool wxGISCatalogMainCmd::GetEnabled(void)
{
    wxCHECK_MSG(m_pGxApp, false, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL && pGxObject->GetParent();
            }
            return false;
		case enumGISCatalogMainCmdConnectFolder:
			return true;
        case enumGISCatalogMainCmdConnectCurrentFolder:
            if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder));
            }
            return false;
		case enumGISCatalogMainCmdDisconnectFolder:
			//check if wxGxDiscConnection
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL && pGxObject->IsKindOf(wxCLASSINFO(wxGxDiscConnection));
            }
			return false;
		case enumGISCatalogMainCmdLocation:
			return true;
		case enumGISCatalogMainCmdDelete:
             if(pCat && pSel)
             {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
					if(pGxObjectEdit && pGxObjectEdit->CanDelete())
						return true;
                }
             }
             return false;
		case enumGISCatalogMainCmdBack:
            if(pSel)
                return pSel->CanUndo();
			return false;
		case enumGISCatalogMainCmdForward:
            if(pSel)
                return pSel->CanRedo();
			return false;
        case enumGISCatalogMainCmdCreateFolder:
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pCont = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if(pCont)
                    return pCont->CanCreate(enumGISContainer, enumContFolder);
            }
			return false;
		case enumGISCatalogMainCmdRename:
            if(pCat && pSel)
            {
                size_t nCounter(0);
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
	                IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
	                if(pGxObjectEdit && pGxObjectEdit->CanRename())
                        nCounter++;
                }
                return nCounter == 1 ? true : false;
            }
            return false;
		case enumGISCatalogMainCmdRefresh:
            if(pSel)
                return pSel->GetCount() > 0;
			return false;
		case enumGISCatalogMainCmdProperties:
			//check if IGxObjectEditUI
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                return  NULL != dynamic_cast<IGxObjectEditUI*>(pGxObject);
            }
			return false;
        case enumGISCatalogMainCmdCopy:
             if(pCat && pSel)
             {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
                    if(pGxObjectEdit && pGxObjectEdit->CanCopy(""))
                        return true;
                }
             }
             return false;
        case enumGISCatalogMainCmdCut:
             if(pCat && pSel)
             {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
                    if(pGxObjectEdit && pGxObjectEdit->CanMove(""))
                        return true;
                }
             }
             return false;
        case enumGISCatalogMainCmdPaste:
            #ifdef __WINDOWS__
        {
            IViewDropTarget* pViewDropTarget = dynamic_cast<IViewDropTarget*>(wxWindow::FindFocus());
            if(pViewDropTarget)
            {
                if( pViewDropTarget->CanPaste() )
                {
                    wxClipboardLocker locker;
                    bool bMove(false);
                    if(!locker)
                    {
                        //
                    }
                    else
                    {

                        wxTextDataObject data;
                        if(wxTheClipboard->GetData( data ))
                        {
                            if(data.GetText() == wxString(wxT("cut")))
                                bMove = true;
                        }
                    }

                    for(size_t i = 0; i < pSel->GetCount(); ++i)
                    {
                        IGxDropTarget* pGxDropTarget = dynamic_cast<IGxDropTarget*>(pCat->GetRegisterObject(pSel->GetSelectedObjectId(i)));
                        if(pGxDropTarget && wxIsDragResultOk(pGxDropTarget->CanDrop(bMove == true ? wxDragMove : wxDragCopy)))
                            return true;
                    }
                }
            }
        }
             return false;
             #else //LINUX
             return true;
             #endif // __WINDOWS__
		case enumGISCatalogMainCmdLinkToClipboard:
        case enumGISCatalogMainCmdSendEmail:
            if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL;
            }
            return false;
		case enumGISCatalogMainCmdConnect:
		    if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
				IGxRemoteConnection *pConn = dynamic_cast<IGxRemoteConnection*>(pGxObject);
                return pConn != NULL && !pConn->IsConnected();
            }
		case enumGISCatalogMainCmdDisconnect:
		    if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
				IGxRemoteConnection *pConn = dynamic_cast<IGxRemoteConnection*>(pGxObject);
                return pConn != NULL && pConn->IsConnected();
            }
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCatalogMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
		case enumGISCatalogMainCmdConnectFolder:
		case enumGISCatalogMainCmdDisconnectFolder:
        case enumGISCatalogMainCmdConnectCurrentFolder:
			return enumGISCommandNormal;
		case enumGISCatalogMainCmdLocation:
			return enumGISCommandControl;
		case enumGISCatalogMainCmdDelete:
			return enumGISCommandNormal;
		case enumGISCatalogMainCmdBack:
		case enumGISCatalogMainCmdForward:
			return enumGISCommandDropDown;
		case enumGISCatalogMainCmdCreateFolder:
		case enumGISCatalogMainCmdRename:
		case enumGISCatalogMainCmdRefresh:
		case enumGISCatalogMainCmdProperties:
		case enumGISCatalogMainCmdCopy:
		case enumGISCatalogMainCmdCut:
		case enumGISCatalogMainCmdPaste:
        case enumGISCatalogMainCmdSendEmail:
		case enumGISCatalogMainCmdConnect:
		case enumGISCatalogMainCmdDisconnect:
		case enumGISCatalogMainCmdLinkToClipboard:
        default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCatalogMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
			return wxString(_("Select parent element"));
		case enumGISCatalogMainCmdConnectFolder:
			return wxString(_("Connect folder"));
        case enumGISCatalogMainCmdConnectCurrentFolder:
            return wxString(_("Connect current folder"));
		case enumGISCatalogMainCmdDisconnectFolder:
			return wxString(_("Disconnect folder"));
		case enumGISCatalogMainCmdLocation:
			return wxString(_("Set or get location"));
		case enumGISCatalogMainCmdDelete:
			return wxString(_("Delete item"));
		case enumGISCatalogMainCmdBack:
			return wxString(_("Go to previous location"));
		case enumGISCatalogMainCmdForward:
			return wxString(_("Go to next location"));
		case enumGISCatalogMainCmdCreateFolder:
			return wxString(_("Create folder"));
		case enumGISCatalogMainCmdRename:
			return wxString(_("Rename item"));
		case enumGISCatalogMainCmdRefresh:
			return wxString(_("Refresh item"));
		case enumGISCatalogMainCmdProperties:
			return wxString(_("Item properties"));
		case enumGISCatalogMainCmdCopy:
			return wxString(_("Copy item(s)"));
		case enumGISCatalogMainCmdCut:
			return wxString(_("Cut item(s)"));
		case enumGISCatalogMainCmdPaste:
			return wxString(_("Paste item(s)"));
        case enumGISCatalogMainCmdSendEmail:
            return wxString(_("Send item(s) by e-mail"));
		case enumGISCatalogMainCmdConnect:
			return wxString(_("Connect"));
        case enumGISCatalogMainCmdDisconnect:
			return wxString(_("Disconnect"));        
        case enumGISCatalogMainCmdLinkToClipboard:
			return wxString(_("Copy link to clipboard"));
		default:
			return wxEmptyString;
	}
}

void wxGISCatalogMainCmd::OnClick(void)
{
    wxCHECK_RET(m_pGxApp && m_pApp, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
			if (NULL != pSel && NULL != pCat)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                if(!pGxObject)
                    return;
				wxGxObject* pParentGxObject = pGxObject->GetParent();
                if(pParentGxObject)
                {
                    pSel->Select(pParentGxObject->GetId(), false, wxGxSelection::INIT_ALL);
                }
                /*
				if(wxIsKindOf(pParentGxObject, wxGxObjectContainer))
                {
                    pSel->Select(pParentGxObject->GetId(), false, wxGxSelection::INIT_ALL);
                }
				else
				{
					wxGxObject* pGrandParentGxObject = pParentGxObject->GetParent();
					pSel->Select(pGrandParentGxObject->GetId(), false, wxGxSelection::INIT_ALL);
				}
                */
            }
			return;
		case enumGISCatalogMainCmdConnectFolder:
		{
			wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder to connect")));
			if (NULL != pSel && NULL != pCat && dlg.ShowModal() == wxID_OK)
			{
                wxGxCatalog* pGxCatalog = wxDynamicCast(pCat, wxGxCatalog);
                if(pGxCatalog)
                {
                    wxGxDiscConnections* pGxDiscConnections = wxDynamicCast(pGxCatalog->GetRootItemByType(wxCLASSINFO(wxGxDiscConnections)), wxGxDiscConnections);
                    if(pGxDiscConnections && pGxDiscConnections->ConnectFolder(dlg.GetPath()))
                        return;
                    else
                        wxGISErrorMessageBox(_("Cannot connect folder"));
                }
			}
			return;
		}
        case enumGISCatalogMainCmdConnectCurrentFolder:
        {
            if (NULL != pSel && NULL != pCat)
            {
                wxGxFolder* pGxFolder = wxDynamicCast(pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId()), wxGxFolder);
                wxGxCatalog* pGxCatalog = wxDynamicCast(pCat, wxGxCatalog);
                if (pGxCatalog && pGxFolder)
                {
                    wxGxDiscConnections* pGxDiscConnections = wxDynamicCast(pGxCatalog->GetRootItemByType(wxCLASSINFO(wxGxDiscConnections)), wxGxDiscConnections);
                    if (pGxDiscConnections && pGxDiscConnections->ConnectFolder(wxString::FromUTF8(pGxFolder->GetPath())))
                        return;
                    else
                        wxGISErrorMessageBox(_("Cannot connect folder"));
                }
            }
            return;
        }
		case enumGISCatalogMainCmdDisconnectFolder:
		{
			if (NULL != pSel && NULL != pCat)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxDiscConnection* pGxDiscConnection = wxDynamicCast(pGxObject, wxGxDiscConnection);
				if (pGxDiscConnection && pGxDiscConnection->Delete())
				{
                    return;
				}
				else
				{
                    wxGISErrorMessageBox(_("Cannot disconnect folder"));
				}
            }
			return;
		}
		case enumGISCatalogMainCmdRename:
			if (NULL != pSel && NULL != pCat)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
				if (pGxView && pGxObject)
				{
                    pGxView->BeginRename(pGxObject->GetId());
				}
            }
			return;
        case enumGISCatalogMainCmdDelete:
			if (NULL != pSel && NULL != pCat)
            {
                bool bAskToDelete(true);

				wxGISAppConfig oConfig = GetConfig();
				bAskToDelete = oConfig.ReadBool(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/catalog/ask_delete")), bAskToDelete);
                if(bAskToDelete)
                {
                    //show ask dialog
                    wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                    wxString sMessage = wxString::Format(_("Do you really want to delete %ld item(s)"), pSel->GetCount());
                    if (pSel->GetCount() > 0)
                    {
                        sMessage.Append(wxT("\n"));
                    }

                    for (size_t i = 0; i < pSel->GetCount(); ++i)
                    {
                        wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                        if (NULL != pGxObject)
                        {
                            sMessage.Append(pGxObject->GetName());
                            if (i % 4 == 0)
                                sMessage.Append(wxT("\n"));
                            else if (i < pSel->GetCount() - 1)
                                sMessage.Append(wxT(", "));
                        }
                    }

                    wxRichMessageDialog dlg(pWnd, sMessage, wxString(_("Delete confirm")), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION | wxSTAY_ON_TOP | wxCENTRE);
					dlg.SetExtendedMessage(wxString(_("The result of operation cannot be undone!\nThe deleted items will remove from disk and will not put to the recycled bin.")));
					dlg.ShowCheckBox("Use my choice and do not show this dialog in future");

					int nRes = dlg.ShowModal();

					if(oConfig.IsOk())
						oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/catalog/ask_delete")), !dlg.IsCheckBoxChecked());

                    if(nRes == wxID_NO)
                        return;
                }

                wxBusyCursor wait;

                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
                    if(pGxObjectEdit && pGxObjectEdit->CanDelete())
                    {
                        wxString sGxObjectName = pGxObject->GetName();
						CPLErrorReset();
                        if(!pGxObjectEdit->Delete())
                        {
                            if(i == pSel->GetCount() - 1)
                            {
								wxString sErrMsg = wxString::Format(_("Delete '%s' failed"), sGxObjectName.c_str());
                                wxGISErrorMessageBox(sErrMsg, wxString::FromUTF8(CPLGetLastErrorMsg()));
                                return;
                            }

                            int nRes = wxMessageBox(wxString::Format(_("Cannot delete '%s'\nContinue?"), sGxObjectName.c_str()), _("Error"), wxYES_NO | wxICON_QUESTION);
                            if(nRes == wxNO)
                                return;
                        }
                    }
                }
            }
            return;
        case enumGISCatalogMainCmdBack:
			if (NULL != pSel && NULL != pCat)
            {
                long nSelId = wxNOT_FOUND;
                if(pSel->CanUndo())
                {
                    nSelId = pSel->Undo(wxNOT_FOUND);
                }
                if(pCat->GetRegisterObject(nSelId))
		        {
			        pSel->Select(nSelId, false, wxGxSelection::INIT_ALL);
		        }
            }
            return;
        case enumGISCatalogMainCmdForward:
			if (NULL != pSel && NULL != pCat)
            {
                long nSelId = wxNOT_FOUND;
                if(pSel->CanRedo())
                {
                    nSelId = pSel->Redo(wxNOT_FOUND);
                }
                if(pCat->GetRegisterObject(nSelId))
		        {
			        pSel->Select(nSelId, false, wxGxSelection::INIT_ALL);
		        }
            }
            return;
        case enumGISCatalogMainCmdRefresh:
			if (NULL != pSel && NULL != pCat)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if(pGxObject)
                        pGxObject->Refresh();
                }
            }
            return;
        case enumGISCatalogMainCmdProperties:
			if (NULL != pSel && NULL != pCat)
            {
				wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                IGxObjectEditUI* pGxObjectEdit = dynamic_cast<IGxObjectEditUI*>(pGxObject);
                if(pGxObjectEdit)
                    pGxObjectEdit->EditProperties(dynamic_cast<wxWindow*>(m_pApp));
            }
            return;
        case enumGISCatalogMainCmdCopy:
			if (NULL != pSel && NULL != pCat)
            {
                wxDataObjectComposite *pDragData = new wxDataObjectComposite();

                wxGISStringDataObject *pNamesData = new wxGISStringDataObject(wxDataFormat(wxGIS_DND_NAME));
                pDragData->Add(pNamesData, true);

                wxFileDataObject *pFileData = new wxFileDataObject();
                pDragData->Add(pFileData, false);

                wxTextDataObject *pTextData = new wxTextDataObject();
                wxString sText;
                pDragData->Add(pTextData, false);

                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if(pGxObject)
                    {
                        wxString sSystemPath(pGxObject->GetPath(), wxConvUTF8);

                        pFileData->AddFile(sSystemPath);
                        pNamesData->AddString(pGxObject->GetFullName());

                        sText.Append(sSystemPath);
                        sText.Append(wxT("\n"));
                    }
                }

                pTextData->SetText(sText);

                //! Lock clipboard
                wxClipboardLocker locker;
                if(!locker)
                    wxGISErrorMessageBox(_("Can't open clipboard"));
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pDragData))
                        wxGISErrorMessageBox(_("Can't copy file(s) to the clipboard"));
                }

            }
    		return;
        case enumGISCatalogMainCmdCut:
			if (NULL != pSel && NULL != pCat)
            {
                wxDataObjectComposite *pDragData = new wxDataObjectComposite();

                wxGISStringDataObject *pNamesData = new wxGISStringDataObject(wxDataFormat(wxGIS_DND_NAME));
                pDragData->Add(pNamesData, true);

                wxFileDataObject *pFileData = new wxFileDataObject();
                pDragData->Add(pFileData, false);

                pDragData->Add(new wxTextDataObject(wxT("cut")));

                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if(pGxObject)
                    {
                        wxString sSystemPath(pGxObject->GetPath(), wxConvUTF8);

                        pFileData->AddFile(sSystemPath);
                        pNamesData->AddString(pGxObject->GetFullName());
                    }
                }
                //! Lock clipboard
                wxClipboardLocker locker;
                if(!locker)
                    wxGISErrorMessageBox(_("Can't open clipboard"));
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pDragData))
                        wxGISErrorMessageBox(_("Can't copy file(s) to the clipboard"));
                }

            }
    		return;
        case enumGISCatalogMainCmdPaste:
			if (NULL != pSel && NULL != pCat)
            {
                IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId()));
                if(!pTarget)
                    return;

                bool bMove(false);

                if(wxTheClipboard->Open())
                {
                    wxTextDataObject data;

                    if(wxTheClipboard->GetData( data ))
                    {
                        if(data.GetText() == wxString(wxT("cut")))
                            bMove = true;
                    }
                    wxTheClipboard->Close();
                }
                else
                {
                     wxGISErrorMessageBox(_("Can't open clipboard"));
                }

                if(wxTheClipboard->Open())
                {
                    wxGISStringDataObject data_names(wxDataFormat(wxGIS_DND_NAME));
                    if(wxTheClipboard->GetData( data_names ))
                    {
                        wxTheClipboard->Close();
                        pTarget->Drop(data_names.GetStrings(), bMove);
                        return;
                    }
                }
                else
                {
                     wxGISErrorMessageBox(_("Can't open clipboard"));
                }

                if(wxTheClipboard->Open())
                {
                    wxFileDataObject filedata;
                    if( wxTheClipboard->GetData( filedata ) )
                    {
                        wxTheClipboard->Close();
                        pTarget->Drop(wxGISDropTarget::PathsToNames(filedata.GetFilenames()), bMove);
                        return;
                    }
                }
                else
                {
                     wxGISErrorMessageBox(_("Can't open clipboard"));
                }

            }
            return;
        case enumGISCatalogMainCmdCreateFolder:
			if (NULL != pSel && NULL != pCat)
            {
                //create folder
                long nSelId = pSel->GetFirstSelectedObjectId();
                wxGxObject* pGxObject = pCat->GetRegisterObject(nSelId);
                wxGxAutoRenamer* pGxFolder = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if(!pGxFolder)
                    return;

                wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());

                CPLString sFolderPath = CheckUniqPath(pGxObject->GetPath(), CPLString(wxString(_("New folder")).ToUTF8()), true, " ");
                pGxFolder->BeginRenameOnAdd(pGxView, sFolderPath);
				CPLErrorReset();
                if(!CreateDir(sFolderPath))
                {
                    wxGISErrorMessageBox(_("Create folder failed!"), wxString::FromUTF8(CPLGetLastErrorMsg()));
                    pGxFolder->BeginRenameOnAdd(NULL, "");
                    return;
                }
            }
            return;
        case enumGISCatalogMainCmdSendEmail:
            if (NULL != pSel && NULL != pCat)
            {
#ifdef wxGIS_USE_EMAIL
                //create temp zip
                CPLString szZipFileName = CPLResetExtension(CPLGenerateTempFilename("email"), "zip");
				CPLErrorReset();
                void* hZIP = CPLCreateZip(szZipFileName, NULL);
                if (!hZIP)
                {
                    wxGISErrorMessageBox(_("Create zip failed!"), wxString::FromUTF8(CPLGetLastErrorMsg()) );
                    CPLError(CE_Failure, CPLE_NoWriteAccess, "ERROR creating %s", szZipFileName.c_str());
                    return;
                }


                //load files to zip
                wxArrayString saPaths;
                for (size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    AddGxObjectToZip(saPaths, hZIP, pGxObject);
                }

                CPLCloseZip(hZIP);

                wxString sContents(_("This e-mail include spatial data in archive : \n"));
                for (size_t i = 0; i < saPaths.GetCount(); ++i)
                {
                    sContents += saPaths[i];
                    sContents.Append(wxT("\r\n"));
                }
                sContents.Append(wxT("--------------------------------------------\n"));
                sContents.Append(_("Created by"));
                sContents.Append(wxT(" "));
                sContents.Append(m_pApp->GetAppDisplayName());
                //send zip via e-mail
                wxString sArchiveName = wxDateTime::Now().Format(wxT("spatial_data_%Y%m%d.zip"));
                wxString sZipFileName(szZipFileName, wxConvUTF8);
                wxMailMessage msg(_("Send: spatial data archive"), sContents, wxEmptyString, sZipFileName, sArchiveName);
                wxEmail email;

                email.Send(msg);
#else
                wxMessageBox(wxString::Format(_("'%s' support was not build"), GetCaption().c_str()), wxString(_("Warning")), wxCENTRE | wxICON_WARNING | wxOK );
#endif //wxGIS_USE_EMAIL                
            }
			return;
		case enumGISCatalogMainCmdLinkToClipboard:
			if (NULL != pSel && NULL != pCat)
            {
				wxTextDataObject *pTextData = new wxTextDataObject();
                wxString sText;
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if(pGxObject)
                    {
                        wxString sSystemPath(pGxObject->GetPath(), wxConvUTF8);

                        sText.Append(sSystemPath);
                        sText.Append(wxT("\n"));
                    }
                }

                pTextData->SetText(sText);
                
                //! Lock clipboard
                wxClipboardLocker locker;
                if(!locker)
                    wxGISErrorMessageBox(_("Can't open clipboard"));
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pTextData))
                        wxGISErrorMessageBox(_("Can't copy file(s) to the clipboard"));
                }
			}
			return;
		case enumGISCatalogMainCmdConnect:		    
			if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
				IGxRemoteConnection *pConn = dynamic_cast<IGxRemoteConnection*>(pGxObject);
                if(pConn != NULL)
					pConn->Connect();
            }
            return;
		case enumGISCatalogMainCmdDisconnect:
			if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
				IGxRemoteConnection *pConn = dynamic_cast<IGxRemoteConnection*>(pGxObject);
                if(pConn != NULL)
					pConn->Disconnect();
            }
            return;
        case enumGISCatalogMainCmdLocation:
		default:
			return;
	}
}

bool wxGISCatalogMainCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
    m_pGxApp = dynamic_cast<wxGxApplicationBase*>(pApp);
    if(m_pApp)
    {
        wxGISAppConfig oConfig = GetConfig();
        if(oConfig.IsOk())
            m_nPrevNextSelCount = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/catalog/prev_next_sel_count")), 7);
    }
	else
	{
        m_nPrevNextSelCount = 7;
	}

	return true;
}

wxString wxGISCatalogMainCmd::GetTooltip(void)
{
    wxCHECK_MSG(m_pGxApp, wxEmptyString, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case enumGISCatalogMainCmdUpOneLevel:
			return wxString(_("Up One Level"));
		case enumGISCatalogMainCmdConnectFolder:
			return wxString(_("Connect folder"));
        case enumGISCatalogMainCmdConnectCurrentFolder:
            return wxString(_("Connect current selected folder"));
		case enumGISCatalogMainCmdDisconnectFolder:
			return wxString(_("Disconnect folder"));
		case enumGISCatalogMainCmdLocation:
			return wxString(_("Set or get location"));
		case enumGISCatalogMainCmdDelete:
			return wxString(_("Delete selected item"));
		case enumGISCatalogMainCmdBack:
            if(pSel && pCat && GetEnabled())
            {
                int nPos = pSel->GetDoPos();
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetDoId(nPos - 1));
                if(pGxObject)
                {
                    wxString sPath = pGxObject->GetFullName();
                    if(!sPath.IsEmpty())
                        return sPath;
                }
            }
			return wxString(_("Go to previous location"));
		case enumGISCatalogMainCmdForward:
            if(pSel && pCat && GetEnabled())
            {
                int nPos = pSel->GetDoPos();
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetDoId(nPos - 1));
                if(pGxObject)
                {
                    wxString sPath = pGxObject->GetFullName();
                    if(!sPath.IsEmpty())
                        return sPath;
                }
            }
			return wxString(_("Go to next location"));
		case enumGISCatalogMainCmdCreateFolder:
			return wxString(_("Create new folder"));
		case enumGISCatalogMainCmdRename:
			return wxString(_("Rename selected item"));
		case enumGISCatalogMainCmdRefresh:
			return wxString(_("Refresh selected item"));
		case enumGISCatalogMainCmdProperties:
			return wxString(_("Show properties of selected item"));
		case enumGISCatalogMainCmdCopy:
			return wxString(_("Copy selected item(s)"));
		case enumGISCatalogMainCmdCut:
			return wxString(_("Cut selected item(s)"));
		case enumGISCatalogMainCmdPaste:
			return wxString(_("Paste selected item(s)"));
		case enumGISCatalogMainCmdSendEmail:
			return wxString(_("Send selected item(s) by e-mail"));
		case enumGISCatalogMainCmdConnect:
			return wxString(_("Connect to remote service"));	
		case enumGISCatalogMainCmdDisconnect:
			return wxString(_("Disconnect from remote service"));			
		case enumGISCatalogMainCmdLinkToClipboard:
			return wxString(_("Copy link to clipboard"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return enumGISCatalogMainCmdMax;
}

IToolBarControl* wxGISCatalogMainCmd::GetControl(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdLocation:
			{
				wxGxLocationComboBox* pGxLocationComboBox = new wxGxLocationComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxSize( 400, 22 ));
				return static_cast<IToolBarControl*>(pGxLocationComboBox);
			}
		default:
			return NULL;
	}
}

wxString wxGISCatalogMainCmd::GetToolLabel(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdLocation:
			return wxString(_("Path")) + wxString(wxT(": "));
		default:
			return wxEmptyString;
	}
}

bool wxGISCatalogMainCmd::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogMainCmdLocation:
			return true;
		default:
			return false;
	}
}

wxMenu* wxGISCatalogMainCmd::GetDropDownMenu(void)
{
    wxCHECK_MSG(m_pGxApp, NULL, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case enumGISCatalogMainCmdBack:
            if(pSel && pCat)
            {
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos > m_nPrevNextSelCount ? nPos - m_nPrevNextSelCount : 0; i < nPos; ++i)// last x steps
                {
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetDoId(i));
                    if(pGxObject)
                    {
                        wxString sPath = pGxObject->GetFullName();
                        if(!sPath.IsEmpty())
                            pMenu->Append(ID_MENUCMD + i, sPath);
                        else
                        {
                            sPath = pGxObject->GetName();
                            if(!sPath.IsEmpty())
                                pMenu->Append(ID_MENUCMD + i, sPath);
                        }
                    }
                }
                return pMenu;
            }
            return NULL;
		case enumGISCatalogMainCmdForward:
            if(pSel && pCat)
            {
                int nPos = pSel->GetDoPos();

                wxMenu* pMenu = new wxMenu();

                for(size_t i = nPos + 1; i < pSel->GetDoSize(); ++i)
                {
                    if(i > nPos + m_nPrevNextSelCount)// last x steps
                        break;
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetDoId(i));
                    if(pGxObject)
                    {
                        wxString sPath = pGxObject->GetFullName();
                        if(!sPath.IsEmpty())
                            pMenu->Append(ID_MENUCMD + i, sPath);
                        else
                        {
                            sPath = pGxObject->GetName();
                            if(!sPath.IsEmpty())
                                pMenu->Append(ID_MENUCMD + i, sPath);
                        }
                    }
                }
                return pMenu;
            }
            return NULL;
		default:
			return NULL;
	}
}

void wxGISCatalogMainCmd::OnDropDownCommand(int nID)
{
    wxCHECK_RET(m_pGxApp, wxT("Application pointer is null"));

    wxGxSelection* pSel = m_pGxApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    if(pSel && pCat && GetEnabled())
    {
        int nPos = pSel->GetDoPos();
        int nNewPos = nID - ID_MENUCMD;
        long nSelId = wxNOT_FOUND;
        if(nNewPos > nPos)
        {
            if(pSel->CanRedo())
            {
                nSelId = pSel->Redo(nNewPos);
            }
        }
        else
        {
            if(pSel->CanUndo())
            {
                nSelId = pSel->Undo(nNewPos);
            }
        }

        if(pCat->GetRegisterObject(nSelId))
		{
			pSel->Select(nSelId, false, wxGxSelection::INIT_ALL);
		}
    }
}

