/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection UI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
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
#pragma once

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxngwconn.h"
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxremoteconnui.h"

#include <wx/hashmap.h>

#ifdef wxGIS_USE_CURL

/** @class wxGxNGWServiceUI

    A NextGIS Web Service GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWServiceUI :
    public wxGxNGWService,
	public IGxObjectUI,
    public IGxObjectEditUI,
    public IGxObjectWizard
{
    DECLARE_CLASS(wxGxNGWServiceUI)
public:
    wxGxNGWServiceUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon, const wxIcon &icLargeIconDsbl = wxNullIcon, const wxIcon &icSmallIconDsbl = wxNullIcon, const wxIcon &icLargeIconGuest = wxNullIcon, const wxIcon &icSmallIconGuest = wxNullIcon);
    virtual ~wxGxNGWServiceUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxNGWService.ContextMenu"));};
    virtual wxString NewMenu(void) const { return wxString(wxT("wxGxNGWService.NewtMenu")); };
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
protected:
    virtual void LoadChildren(void);
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
    wxIcon m_icLargeIconDsbl, m_icSmallIconDsbl;
    wxIcon m_icLargeIconGuest, m_icSmallIconGuest;
};

typedef struct _iconset{
	wxIcon icSmallIcon;
	wxIcon icLargeIcon;
} ICONSET;

WX_DECLARE_STRING_HASH_MAP( ICONSET, wxIconSetMap );

/** @class wxGxNGWResourceGroupUI

    A NextGIS Web Service Resource Group GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWResourceGroupUI :
    public wxGxNGWResourceGroup,
    public IGxObjectUI,
	public wxGxAutoRenamer,
    public IGxDropTarget,
    public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxNGWResourceGroupUI)
public:
    wxGxNGWResourceGroupUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon);
    virtual ~wxGxNGWResourceGroupUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
    virtual wxString ContextMenu(void) const { return wxString(wxT("wxGxNGWResourceGroup.ContextMenu")); };
    virtual wxString NewMenu(void) const { return wxString(wxT("wxGxNGWResourceGroup.NewMenu")); };
    //IGxDropTarget
    virtual bool Drop(const wxArrayString& saGxObjectPaths, bool bMove);
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
	virtual bool CanImport();
	virtual bool Import(wxWindow* pWnd);
	//
	virtual bool CreateVectorLayer(const wxString &sName, wxGISDataset * const pInputDataset, OGRwkbGeometryType eFilterGeomType, const wxString& sStyle = wxEmptyString, bool bSkipInvalidGeometry = true, ITrackCancel* const pTrackCancel = NULL);
    virtual bool CreateRasterLayer(const wxString &sName, wxGISDataset * const pInputDataset, unsigned char R, unsigned char G, unsigned char B, unsigned char A, bool bAutoCrop, ITrackCancel* const pTrackCancel);
	
protected:
    virtual wxGxObject* AddResource(const wxJSONValue &Data);
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
	wxIcon m_icFolderLargeIcon, m_icFolderSmallIcon;
	wxIcon m_icPGLayerLargeIcon, m_icPGLayerSmallIcon;
	wxIcon m_icPGConnLargeIcon, m_icPGConnSmallIcon;
	wxIcon m_icPGDisConnLargeIcon, m_icPGDisConnSmallIcon;
	wxIcon m_icNGWLayerLargeIcon, m_icNGWLayerSmallIcon;
	wxIcon m_icNGWRasterLargeIcon, m_icNGWRasterSmallIcon;
	wxIconSetMap m_stmIconSet;
	
    //wxIcon m_icNGWPackageLargeIcon, m_icNGWPackageSmallIcon;
};


/** @class wxGxNGWRootResourceUI

    A NextGIS Web Service Root resource GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWRootResourceUI :
    public wxGxNGWResourceGroupUI
{
    DECLARE_CLASS(wxGxNGWRootResourceUI)
public:
    wxGxNGWRootResourceUI(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon);
    virtual ~wxGxNGWRootResourceUI(void);
    //IGxObjectUI
    virtual wxIcon GetLargeImage(void);
    virtual wxIcon GetSmallImage(void);
    virtual wxString ContextMenu(void) const { return wxString(wxT("wxGxNGWResourceGroup.ContextMenu")); };
    virtual wxString NewMenu(void) const { return wxString(wxT("wxGxNGWResourceGroup.NewMenu")); };
};

/** @class wxGxNGWLayerUI

    A NextGIS Web Service Layer GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWLayerUI :
    public wxGxNGWLayer,
    public IGxObjectUI,
    public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxNGWLayerUI)
public:
    wxGxNGWLayerUI(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon);
    virtual ~wxGxNGWLayerUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
    virtual wxString ContextMenu(void) const { return wxString(wxT("wxGxNGWLayer.ContextMenu")); };
    virtual wxString NewMenu(void) const { return wxEmptyString; };
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
};


/** @class wxGxNGWRasterUI

    A NextGIS Web Service Raster GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWRasterUI :
    public wxGxNGWRaster,
    public IGxObjectUI,
    public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxNGWRasterUI)
public:
    wxGxNGWRasterUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon);
    virtual ~wxGxNGWRasterUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
    virtual wxString ContextMenu(void) const { return wxString(wxT("wxGxNGWRaster.ContextMenu")); };
    virtual wxString NewMenu(void) const { return wxEmptyString; };
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
};

/** @class wxGxNGWPostGISConnectionUI

    A NextGIS Web Service PostGIS Connection GxObjectUI.

    @library{catalogui}
*/
#ifdef wxGIS_USE_POSTGRES
class WXDLLIMPEXP_GIS_CLU wxGxNGWPostGISConnectionUI :
    public wxGxRemoteConnectionUI,
    public wxGxNGWResource
{
    DECLARE_CLASS(wxGxNGWPostGISConnectionUI)
public:
    wxGxNGWPostGISConnectionUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIconConn = wxNullIcon, const wxIcon &SmallIconConn = wxNullIcon, const wxIcon &LargeIconDisconn = wxNullIcon, const wxIcon &SmallIconDisconn = wxNullIcon);
    virtual ~wxGxNGWPostGISConnectionUI(void);
    //wxGxObject
    virtual wxString GetCategory(void) const;    
	//IGxObjectEdit
	virtual bool Delete(void);
    virtual bool CanDelete(void);
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool CanMove(const CPLString &szDestPath);
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
	virtual int GetParentResourceId() const;
protected:
	wxString m_sUser, m_sPass, m_sDatabase, m_sHost;
};
#endif //wxGIS_USE_POSTGRES

/** @class wxGxNGWFileSetUI

    A NextGIS Web Service File set GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxNGWFileSetUI :
    public wxGxNGWFileSet,
    public IGxObjectUI,
    public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxNGWFileSetUI)
public:
    wxGxNGWFileSetUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &icLargeIcon = wxNullIcon, const wxIcon &icSmallIcon = wxNullIcon);
    virtual ~wxGxNGWFileSetUI(void);
    //IGxObjectUI
    virtual wxIcon GetLargeImage(void);
    virtual wxIcon GetSmallImage(void);
    virtual wxString ContextMenu(void) const { return wxString(wxT("wxGxNGWFileSet.ContextMenu")); };
    virtual wxString NewMenu(void) const { return wxEmptyString; };
    //IGxObjectEditUI    
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_icLargeIcon, m_icSmallIcon;
};


#endif // wxGIS_USE_CURL
