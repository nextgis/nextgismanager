/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxXMLConnectionStorage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012,2013 Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/core/json/jsonval.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/json/jsonwriter.h"

#include <wx/event.h>
#include <wx/fswatcher.h>

#if wxVERSION_NUMBER <= 2903// && !defined EVT_FSWATCHER(winid, func)
#define EVT_FSWATCHER(winid, func) \
    wx__DECLARE_EVT1(wxEVT_FSWATCHER, winid, wxFileSystemWatcherEventHandler(func))
#endif

/** \class wxGxXMLConnectionStorage gxremoteservers.h
    \brief The class support for storing and updating xml file connections.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxXMLConnectionStorage :
	public wxGxObjectContainer
{
    DECLARE_ABSTRACT_CLASS(wxGxXMLConnectionStorage)
public:
    wxGxXMLConnectionStorage(void);
    ~wxGxXMLConnectionStorage(void);
    //events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
protected:
    virtual void LoadConnectionsStorage(void);
    virtual bool IsObjectExist(wxGxObject* const pObj, const wxXmlNode* pNode) = 0;
    virtual void CreateConnectionsStorage(void) = 0;
    virtual wxGxObject* CreateChildGxObject(const wxXmlNode* pNode) = 0;
    virtual int GetStorageVersion(void) const = 0;
protected:
    wxString m_sXmlStorageName;
    wxString m_sXmlStoragePath;
    wxCriticalSection m_oCritSect;
};
*/

/** \class wxGxJSONConnectionStorage gxremoteservers.h
    \brief The class support for storing and updating json file connections.
*/

class WXDLLIMPEXP_GIS_CLT wxGxJSONConnectionStorage :
	public wxGxObjectContainer
{
    DECLARE_ABSTRACT_CLASS(wxGxJSONConnectionStorage)
public:
    wxGxJSONConnectionStorage(void);
    ~wxGxJSONConnectionStorage(void);
    virtual bool AddItem(int nStoreId, const wxString& sName, const wxString& sPath);
    virtual bool DeleteItemById(int nStoreId);
    virtual bool RenameItemById(int nStoreId, const wxString& NewName);
    //events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
protected:
    virtual void LoadConnectionsStorage(void);
    virtual bool IsObjectExist(wxGxObject* const pObj, const wxJSONValue& GxObjConfValue) = 0;
    virtual void CreateConnectionsStorage(void) = 0;
    virtual wxGxObject* CreateChildGxObject(const wxJSONValue& GxObjConfValue) = 0;
    virtual int GetStorageVersion(void) const = 0;
protected:
    wxString m_sStorageName;
    wxString m_sStoragePath;
    wxCriticalSection m_oCritSect;
};
