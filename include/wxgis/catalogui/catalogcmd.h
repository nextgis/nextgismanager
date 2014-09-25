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
#pragma once

#include "wxgis/framework/command.h"
#include "wxgis/catalogui/gxapplication.h"

enum wxGISEnumCatalogMainCmdType{
    enumGISCatalogMainCmdUpOneLevel = 0,
	enumGISCatalogMainCmdConnectFolder,
	enumGISCatalogMainCmdDisconnectFolder,
	enumGISCatalogMainCmdLocation,
	enumGISCatalogMainCmdDelete,
	enumGISCatalogMainCmdBack,
	enumGISCatalogMainCmdForward,
	enumGISCatalogMainCmdCreateFolder,
	enumGISCatalogMainCmdRename,
	enumGISCatalogMainCmdRefresh,
	enumGISCatalogMainCmdProperties,
	enumGISCatalogMainCmdCopy,
	enumGISCatalogMainCmdCut,
	enumGISCatalogMainCmdPaste,
	enumGISCatalogMainCmdSendEmail,
	enumGISCatalogMainCmdConnect,
	enumGISCatalogMainCmdDisconnect,
    enumGISCatalogMainCmdMax
};

/** @class wxGISCatalogMainCmd
  *
  * The main catalog commands includes: "Up One Level", "Connect Folder", "Disconnect Folder", "Location", "Delete Item", "Back", "Forward", "Create Folder", "Rename", "Refresh", "Properties", "Copy", "Cut" and "Paste".
  * 
  * @library{catalogui}
  */

class wxGISCatalogMainCmd :
    public wxGISCommand,
	public IToolControl,
    public IDropDownCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISCatalogMainCmd)

public:
	wxGISCatalogMainCmd(void);
	virtual ~wxGISCatalogMainCmd(void);
	//wxGISCommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(wxGISApplicationBase* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
	//IToolControl
	virtual IToolBarControl* GetControl(void);
	virtual wxString GetToolLabel(void);
	virtual bool HasToolLabel(void);
    //IDropDownCommand
   	virtual wxMenu* GetDropDownMenu(void);
    virtual void OnDropDownCommand(int nID);
protected:
    bool AddGxObjectToZip(wxArrayString &saPaths, void* hZIP, wxGxObject* pGxObject, const CPLString &szPath = "");
    void AddFileToZip(const CPLString &szPath, void* hZIP, GByte **pabyBuffer, size_t nBufferSize, const CPLString &szPrependPath, const wxString &sCharset);
protected:
	wxGISApplicationBase* m_pApp;
	wxGxApplicationBase* m_pGxApp;
	wxIcon m_IconFolderUp, m_IconFolderConn, m_IconFolderConnDel, m_IconDel, m_IconGoPrev, m_IconGoNext;
    wxIcon m_IconFolderNew, m_IconEdit, m_IconViewRefresh, m_IconProps;
    wxIcon m_LargeFolderIcon, m_SmallFolderIcon;
    wxIcon m_CopyIcon, m_CutIcon, m_PasteIcon;
    wxIcon m_EmailIcon, m_ConnectIcon;
    short m_nPrevNextSelCount;
};
