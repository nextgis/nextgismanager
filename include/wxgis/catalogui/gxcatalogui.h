/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalogUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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
#pragma once

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxcatalog.h"

#include <wx/imaglist.h>

/** \class wxGxCatalogUI gxcatalogui.h
    \brief The GxCatalogUI class.
*/
class  WXDLLIMPEXP_GIS_CLU wxGxCatalogUI :
    public wxGxCatalog,
    public IGxObjectUI,
	public IGxObjectEditUI
{
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGxCatalogUI);
public:
    wxGxCatalogUI(wxGxObject *oParent = NULL, const wxString &soName = _("Catalog"), const CPLString &soPath = "");
    virtual ~wxGxCatalogUI(void);
    virtual void SetOpenLastPath(bool bOpenLast) {m_bOpenLastPath = bOpenLast;};
    virtual bool GetOpenLastPath(void){return m_bOpenLastPath;};
    virtual long AddPending(long nParentId);
    virtual void RemovePending(long nPendingId);
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxCatalog.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxCatalog.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
	virtual wxString GetConfigName(void) const {return wxString(wxT("wxCatalogUI"));};
protected:
    wxIcon m_oIcon;
    bool m_bOpenLastPath;
    wxVector<wxIcon> m_oaPendingIconsLarge, m_oaPendingIconsSmall;
};

bool WXDLLIMPEXP_GIS_CLU FolderDrop(const CPLString& pPath, const wxArrayString& GxObjectPaths, bool bMove);

/*

class WXDLLIMPEXP_GIS_CLU wxGxCatalogUI :
    public wxGxCatalog,
    public IGxObjectUI,
	public IGxObjectEditUI
{
public:
	wxGxCatalogUI(bool bFast = false);
	virtual ~wxGxCatalogUI(void);
    //IGxObject
	virtual void Detach(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxCatalog.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxCatalog.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
 	//IGxCatalog
	virtual void ObjectDeleted(long nObjectID);
	virtual IGxObject* ConnectFolder(wxString sPath, bool bSelect = true);
	virtual void DisconnectFolder(CPLString sPath);
    //wxGxCatalogUI
    virtual void Undo(int nPos = wxNOT_FOUND);
    virtual void Redo(int nPos = wxNOT_FOUND);
	virtual void SetLocation(wxString sPath);
    virtual void SetOpenLastPath(bool bOpenLast) {m_bOpenLastPath = bOpenLast;};
    virtual bool GetOpenLastPath(void){return m_bOpenLastPath;};
	virtual IGxSelection* GetSelection(void){return m_pSelection;};
    virtual wxImageList* GetPendingImageList(bool bIsLarge)
    {
        if(bIsLarge)
            return &m_ImageListLarge;
        else
            return &m_ImageListSmall;
    }
	//wxGxCatalog
	virtual wxString GetConfigName(void){return wxString(wxT("wxCatalogUI"));};
	//
	virtual void Init(IGxCatalog* pExtCat = NULL);
protected:
	bool m_bOpenLastPath, m_bHasInternal;
	IGxSelection* m_pSelection;
    wxImageList m_ImageListSmall, m_ImageListLarge;
    IGxCatalog* m_pExtCat;
};
*/