/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
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

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxobjectfactory.h"
#include "wxgis/core/pointer.h"

#include <wx/xml/xml.h>
#include <wx/event.h>
#include <wx/fswatcher.h>

#if wxVERSION_NUMBER <= 2903// && !defined EVT_FSWATCHER(winid, func)
#define EVT_FSWATCHER(winid, func) \
    wx__DECLARE_EVT1(wxEVT_FSWATCHER, winid, wxFileSystemWatcherEventHandler(func))
#endif

/** @class wxGxCatalog

    The main catalog class. Catalog stores and provides access to the tree of geodata objects (GxObjects)

    @library{catalog}
*/
class WXDLLIMPEXP_GIS_CLT wxGxCatalog :
    public wxGxCatalogBase,
	public wxGISConnectionPointContainer
{
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGxCatalog);
public:
    wxGxCatalog(wxGxObject *oParent = NULL, const wxString &soName = _("Catalog"), const CPLString &soPath = "");
    virtual ~wxGxCatalog(void);
    virtual void ObjectAdded(long nObjectID);
	virtual void ObjectChanged(long nObjectID);
	virtual void ObjectDeleted(long nObjectID);
	virtual void ObjectRefreshed(long nObjectID);
    virtual bool CreateChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds);
    virtual void EnableRootItem(size_t nItemId, bool bEnable);
    virtual bool Destroy(void);
    virtual wxGxObject* const GetRootItemByType(const wxClassInfo * info) const;
    virtual wxGxObjectFactory* const GetObjectFactoryByClassName(const wxString &sClassName);
    virtual wxGxObjectFactory* const GetObjectFactoryByName(const wxString &sFactoryName);

    typedef struct _root_item{
        wxString sClassName;
        wxString sName;
        bool bEnabled;
        wxXmlNode* pConfig;
    } ROOTITEM;

    virtual wxVector<wxGxCatalog::ROOTITEM>* const GetRootItems(void);
    virtual wxVector<wxGxObjectFactory*>* const GetObjectFactories(void);
    //watcher
    virtual bool AddFSWatcherPath(const wxFileName& path, int events = wxFSW_EVENT_ALL);
    virtual bool AddFSWatcherTree(const wxFileName& path, int events = wxFSW_EVENT_ALL, const wxString& filespec = wxEmptyString);
    virtual bool RemoveFSWatcherPath(const wxFileName& path);
    virtual bool RemoveFSWatcherTree(const wxFileName& path);
//events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
protected:
    //wxGxCatalogBase
	virtual void LoadObjectFactories(const wxXmlNode* pNode);
	virtual void LoadObjectFactories(void);
    virtual void LoadChildren(void);
    virtual void LoadChildren(wxXmlNode* const pNode);
	virtual void EmptyObjectFactories(void);
    virtual void SerializePlugins(wxXmlNode* pNode, bool bStore = false);
protected:
	virtual wxString GetConfigName(void) const {return wxString(wxT("wxCatalog"));};
	virtual bool IsPathWatched(const wxString& sPath);
protected:
    wxArrayString m_CatalogRootItemArray;
    wxVector<wxGxObjectFactory*> m_ObjectFactoriesArray;
    wxVector<ROOTITEM> m_staRootitems;

    wxFileSystemWatcher *m_pWatcher;
    wxArrayString m_asWatchPaths;
//    wxCriticalSection m_oCritSect;
private:
    DECLARE_EVENT_TABLE()
};

#define wxGIS_GXCATALOG_EVENT(x)  {  wxGxCatalogBase* pGxCatalog = GetGxCatalog(); \
    if(pGxCatalog) pGxCatalog->x(GetId()); }
#define wxGIS_GXCATALOG_EVENT_ID(x, id)   { wxGxCatalogBase* pGxCatalog = GetGxCatalog(); \
    if(pGxCatalog) pGxCatalog->x(id); }


