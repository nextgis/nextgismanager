/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/richmsgdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/mimetype.h>
#include <wx/uri.h>

//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder - duplicate Delete command 
//	3	Location
//  4   Delete Item
//  5   Back
//  6   Forward
//  7   Create Folder - need in object dialog
//	8	Rename
//	9	Refresh
//  10  Properties
//  11  Copy
//  12  Cut
//  13  Paste
//  14  send to email
//  15  ?

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
		case 0:
			if(!m_IconFolderUp.IsOk())
				m_IconFolderUp = wxIcon(folder_up_xpm);
			return m_IconFolderUp;
		case 1:
			if(!m_IconFolderConn.IsOk())
				m_IconFolderConn = wxIcon(folder_conn_new_xpm);
			return m_IconFolderConn;
		case 2:
			if(!m_IconFolderConnDel.IsOk())
				m_IconFolderConnDel = wxIcon(folder_conn_del_xpm);
			return m_IconFolderConnDel;
		case 4:
			if(!m_IconDel.IsOk())
				m_IconDel = wxIcon(delete_xpm);
			return m_IconDel;
		case 5:
			if(!m_IconGoPrev.IsOk())
			{
				wxBitmap oGoOrigin(go_xpm);
				wxImage oGoPrevious = oGoOrigin.ConvertToImage();
				oGoPrevious = oGoPrevious.Mirror(true);

				m_IconGoPrev.CopyFromBitmap(wxBitmap(oGoPrevious));
			}
			return m_IconGoPrev;
		case 6:
			if(!m_IconGoNext.IsOk())
				m_IconGoNext = wxIcon(go_xpm);
			return m_IconGoNext;
		case 7:
			if(!m_IconFolderNew.IsOk())
				m_IconFolderNew = wxIcon(folder_new_xpm);
			return m_IconFolderNew;
		case 8:
			if(!m_IconEdit.IsOk())
				m_IconEdit = wxIcon(edit_xpm);
			return m_IconEdit;
		case 9:
			if(!m_IconViewRefresh.IsOk())
				m_IconViewRefresh = wxIcon(view_refresh_xpm);
			return m_IconViewRefresh;
		case 10:
			if(!m_IconProps.IsOk())
				m_IconProps = wxIcon(properties_xpm);
			return m_IconProps;
		case 11:
			if(!m_CopyIcon.IsOk())
				m_CopyIcon = wxIcon(edit_copy_xpm);
			return m_CopyIcon;
		case 12:
			if(!m_CutIcon.IsOk())
				m_CutIcon = wxIcon(edit_cut_xpm);
			return m_CutIcon;
		case 13:
			if(!m_PasteIcon.IsOk())
				m_PasteIcon = wxIcon(edit_paste_xpm);
			return m_PasteIcon;
		case 14:
            if (!m_EmailIcon.IsOk())
                m_EmailIcon = wxIcon(email_xpm);
            return m_EmailIcon;
		case 3:
		default:
			return wxNullIcon;
	}
}

wxString wxGISCatalogMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Up One Level"));
		case 1:
			return wxString(_("&Connect folder"));
		case 2:
			return wxString(_("&Disconnect folder"));
		case 3:
			return wxString(_("Location"));
		case 4:
			return wxString(_("Delete"));
		case 5:
			return wxString(_("Back"));
		case 6:
			return wxString(_("Forward"));
		case 7:
			return wxString(_("Create folder"));
		case 8:
			return wxString(_("Rename"));
		case 9:
			return wxString(_("Refresh"));
		case 10:
			return wxString(_("Properties"));
		case 11:
			return wxString(_("Copy"));
		case 12:
			return wxString(_("Cut"));
		case 13:
			return wxString(_("Paste"));
		case 14:
			return wxString(_("e-mail to..."));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCatalogMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 5:
		case 6:
		case 9:
			return wxString(_("Catalog"));
		case 7:
			return wxString(_("Create"));
		case 4:
		case 8:
		case 11:
		case 12:
		case 13:
			return wxString(_("Edit"));
		case 10:
			return wxString(_("Miscellaneous"));
		case 14:
			return wxString(_("Send to"));
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
		case 0://Up One Level
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL && pGxObject->GetParent();
            }
            return false;
		case 1:
			return true;
		case 2:
			//check if wxGxDiscConnection
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL && pGxObject->IsKindOf(wxCLASSINFO(wxGxDiscConnection));
            }
			return false;
		case 3:
			return true;
		case 4://delete
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
		case 5:
            if(pSel)
                return pSel->CanUndo();
			return false;
		case 6:
            if(pSel)
                return pSel->CanRedo();
			return false;
        case 7:
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                wxGxObjectContainer* pCont = wxDynamicCast(pGxObject, wxGxObjectContainer);
                if(pCont)
                    return pCont->CanCreate(enumGISContainer, enumContFolder);
            }
			return false;
		case 8://Rename
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
		case 9://Refresh
            if(pSel)
                return pSel->GetCount() > 0;
			return false;
		case 10://Properties
			//check if IGxObjectEditUI
            if(pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                return  NULL != dynamic_cast<IGxObjectEditUI*>(pGxObject);
            }
			return false;
        case 11://Copy
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
        case 12://Cut
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
        case 13://Paste
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
        case 14://e-mail
            if (pCat && pSel)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());
                return pGxObject != NULL;
            }
            return false;

		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCatalogMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Up One Level
		case 1://Connect folder
		case 2://Disconnect folder
			return enumGISCommandNormal;
		case 3://location
			return enumGISCommandControl;
		case 4://delete
			return enumGISCommandNormal;
		case 5://back
		case 6://forward
			return enumGISCommandDropDown;
		case 7://Create folder
		case 8://Rename
		case 9://Refresh
		case 10://Properties
		case 11://copy
		case 12://cut
		case 13://paste
        case 14://e-mail
        default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCatalogMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Select parent element"));
		case 1:
			return wxString(_("Connect folder"));
		case 2:
			return wxString(_("Disconnect folder"));
		case 3:
			return wxString(_("Set or get location"));
		case 4:
			return wxString(_("Delete item"));
		case 5:
			return wxString(_("Go to previous location"));
		case 6:
			return wxString(_("Go to next location"));
		case 7:
			return wxString(_("Create folder"));
		case 8:
			return wxString(_("Rename item"));
		case 9:
			return wxString(_("Refresh item"));
		case 10:
			return wxString(_("Item properties"));
		case 11:
			return wxString(_("Copy item(s)"));
		case 12:
			return wxString(_("Cut item(s)"));
		case 13:
			return wxString(_("Paste item(s)"));
        case 14:
            return wxString(_("Send item(s) by e-mail"));
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
		case 0:
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
			break;
		case 1://	1	Connect Folder
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
                        wxMessageBox(_("Cannot connect folder"), _("Error"), wxOK | wxICON_ERROR);
                }
			}
			return;
		}
		case 2://	2	Disconnect Folder - duplicate Delete command 
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
                    wxMessageBox(_("Cannot disconnect folder"), _("Error"), wxOK | wxICON_ERROR);
				}
            }
			return;
		}
		case 8:
			if (NULL != pSel && NULL != pCat)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
				if (pGxView && pGxObject)
				{
                    pGxView->BeginRename(pGxObject->GetId());
				}
            }
			break;
        case 4:
			if (NULL != pSel && NULL != pCat)
            {
                bool bAskToDelete(true);

				wxGISAppConfig oConfig = GetConfig();
				bAskToDelete = oConfig.ReadBool(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/catalog/ask_delete")), bAskToDelete);
                if(bAskToDelete)
                {
                    //show ask dialog
                    wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                    wxString sMessage = wxString::Format(_("Do you really want to delete %d item(s)"), pSel->GetCount());
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
                        if(!pGxObjectEdit->Delete())
                        {
                            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                            if(i == pSel->GetCount() - 1)
                            {
                                wxMessageBox(wxString::Format(_("Delete '%s' failed"), sGxObjectName.c_str()), _("Error"), wxOK | wxICON_ERROR, pWnd);
                                return;
                            }

                            int nRes = wxMessageBox(wxString::Format(_("Cannot delete '%s'\nContinue?"), sGxObjectName.c_str()), _("Error"), wxYES_NO | wxICON_QUESTION, pWnd);
                            if(nRes == wxNO)
                                return;
                        }
                    }
                }
            }
            return;
        case 5:
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
        case 6:
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
        case 9:
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
        case 10:
			if (NULL != pSel && NULL != pCat)
            {
				wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetLastSelectedObjectId());
                IGxObjectEditUI* pGxObjectEdit = dynamic_cast<IGxObjectEditUI*>(pGxObject);
                if(pGxObjectEdit)
                    pGxObjectEdit->EditProperties(dynamic_cast<wxWindow*>(m_pApp));
            }
            return;
        case 11://copy
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
                    wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pDragData))
                        wxMessageBox(_("Can't copy file(s) to the clipboard"), _("Error"), wxOK | wxICON_ERROR);
                }

            }
    		return;
        case 12://cut
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
                    wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                else
                {
                    //! Put data to clipboard
                    if(!wxTheClipboard->AddData(pDragData))
                        wxMessageBox(_("Can't copy file(s) to the clipboard"), _("Error"), wxOK | wxICON_ERROR);
                }

            }
    		return;
        case 13://paste
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
                     wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
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
                     wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
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
                     wxMessageBox(_("Can't open clipboard"), _("Error"), wxOK | wxICON_ERROR);
                }

            }
            return;
        case 7:
			if (NULL != pSel && NULL != pCat)
            {
                //create folder
                long nSelId = pSel->GetFirstSelectedObjectId();
                wxGxObject* pGxObject = pCat->GetRegisterObject(nSelId);
                wxGxAutoRenamer* pGxFolder = dynamic_cast<wxGxAutoRenamer*>(pGxObject);
                if(!pGxFolder)
                    return;

                wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());

                CPLString sFolderPath = CheckUniqPath(pGxObject->GetPath(), CPLString(wxString(_("New folder")).mb_str(wxConvUTF8)), true, " ");
                pGxFolder->BeginRenameOnAdd(pGxView, sFolderPath);
                if(!CreateDir(sFolderPath))
                {
                    wxMessageBox(_("Create folder failed!"), _("Error"), wxICON_ERROR | wxOK );
                    pGxFolder->BeginRenameOnAdd(NULL, "");
                    return;
                }
            }
            return;
        case 14:
            if (NULL != pSel && NULL != pCat)
            {
#ifdef wxGIS_USE_EMAIL
                //create temp zip
                CPLString szZipFileName = CPLResetExtension(CPLGenerateTempFilename("email"), "zip");
                void* hZIP = CPLCreateZip(szZipFileName, NULL);

                if (!hZIP) 
                {
                    wxMessageBox(_("Create zip failed!"), _("Error"), wxICON_ERROR | wxOK );
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

                wxString sContents(_("This e-mail include spatial data in archive : \r\n"));
                for (size_t i = 0; i < saPaths.GetCount(); ++i)
                {
                    sContents += saPaths[i];
                    sContents.Append(wxT("\r\n"));
                }
                sContents.Append(wxT("--------------------------------------------\r\n"));
                sContents.Append(_("Created by NextGIS Manager"));
                //send zip via e-mail
                wxString sArchiveName = wxDateTime::Now().Format(wxT("spatial_data_%Y%m%d.zip"));
                wxMailMessage msg(_("Send: spatial data archive"), sContents, wxEmptyString, wxString(szZipFileName, wxConvUTF8), sArchiveName);
                wxEmail email;

                email.Send(msg);
#endif //wxGIS_USE_EMAIL
                return;
            }
        case 3:
		default:
			return;
	}
}

bool wxGISCatalogMainCmd::AddGxObjectToZip(wxArrayString &saPaths, void* hZIP, wxGxObject* pGxObject, const CPLString &szPath)
{
    if (NULL == pGxObject)
    {
        return false;
    }

    if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
    {
        wxGxDataset* pGxDS = wxDynamicCast(pGxObject, wxGxDataset);
        if (NULL == pGxDS)
        {
            return false;
        }

        
        if (!IsFileDataset(pGxDS->GetType(), pGxDS->GetSubType()))
        {
            return false;
        }

        wxString sCharset(wxT("cp-866"));
        wxGISAppConfig oConfig = GetConfig();
        if (oConfig.IsOk())
            sCharset = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/zip/charset")), sCharset);

        wxString sName = pGxDS->GetName();
        saPaths.Add(sName);

        wxGISDataset* pDS = pGxDS->GetDataset(false);
        if (NULL == pDS)
        {
            return false;
        }

        VSILFILE *fp;
        size_t nBufferSize = 1024 * 1024;
        GByte *pabyBuffer = (GByte *)CPLMalloc(nBufferSize);
        size_t nBytesRead;
        
        char** papszFileList = pDS->GetFileList();
        papszFileList = CSLAddString(papszFileList, pDS->GetPath());
        for (int i = 0; papszFileList[i] != NULL; ++i)
        {
            int nRet = 0;

            CPLString szName;
            if (szPath.empty())
            {
                szName = CPLGetFilename(papszFileList[i]);
            }
            else
            {
                szName += szPath;
                szName += "/";
                szName += CPLGetFilename(papszFileList[i]);
            }

            szName = CPLString(wxString(szName, wxConvUTF8).mb_str(wxCSConv(sCharset)));

            fp = VSIFOpenL(papszFileList[i], "rb");
            if (fp == NULL)
                continue;

            if (CPLCreateFileInZip(hZIP, szName, NULL) == CE_None)
            {
                do {
                    nBytesRead = VSIFReadL(pabyBuffer, 1, nBufferSize, fp);
                    if (long(nBytesRead) < 0)
                        nRet = -1;

                    if (nRet == 0 && CPLWriteFileInZip(hZIP, pabyBuffer, nBytesRead) != CE_None)
                        nRet = -1;
                } while (nRet == 0 && nBytesRead == nBufferSize);

            }
                
            //    CPLError(CE_Failure, CPLE_FileIO, "ERROR adding %s to zip", szName);
            CPLCloseFileInZip(hZIP);
            VSIFCloseL(fp);
        }

        CPLFree(pabyBuffer);
        CSLDestroy(papszFileList);
    }
    else if (pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
    {
        CPLString szNewPath;
        if (szPath.empty())
            szNewPath = CPLString(pGxObject->GetName().mb_str(wxConvUTF8));
        else
            szNewPath = szPath + "/" + CPLString(pGxObject->GetName().mb_str(wxConvUTF8));
        wxGxObjectContainer* pCont = wxDynamicCast(pGxObject, wxGxObjectContainer);
        if (pCont && pCont->HasChildren())
        {
            const wxGxObjectList lObj = pCont->GetChildren();
            for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
            {
                AddGxObjectToZip(saPaths, hZIP, *it, szNewPath);
            }
        }
    }
    return true;
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
		case 0:
			return wxString(_("Up One Level"));
		case 1:
			return wxString(_("Connect folder"));
		case 2:
			return wxString(_("Disconnect folder"));
		case 3:
			return wxString(_("Set or get location"));
		case 4:
			return wxString(_("Delete selected item"));
		case 5:
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
		case 6:
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
		case 7:
			return wxString(_("Create new folder"));
		case 8:
			return wxString(_("Rename selected item"));
		case 9:
			return wxString(_("Refresh selected item"));
		case 10:
			return wxString(_("Show properties of selected item"));
		case 11:
			return wxString(_("Copy selected item(s)"));
		case 12:
			return wxString(_("Cut selected item(s)"));
		case 13:
			return wxString(_("Paste selected item(s)"));
		case 14:
			return wxString(_("Send selected item(s) by e-mail"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogMainCmd::GetCount(void)
{
	return 15;
}

IToolBarControl* wxGISCatalogMainCmd::GetControl(void)
{
	switch(m_subtype)
	{
		case 3:
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
		case 3:
			return wxString(_("Path:   "));
		default:
			return wxEmptyString;
	}
}

bool wxGISCatalogMainCmd::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case 3:
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
		case 5:
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
		case 6:
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

