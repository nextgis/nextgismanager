/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2010,2012-2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxevent.h"

#define TM_CHECKING 950

/** @class wxGxDiscConnection

    A Disc Connection GxObject.

    @library{catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxDiscConnection :
	public wxGxFolder
{
    DECLARE_DYNAMIC_CLASS(wxGxDiscConnection)
	enum
    {
        TIMER_ID = 1016
    };
public:
    wxGxDiscConnection(void);
	wxGxDiscConnection(wxGxObject *oParent, int nStoreId, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxDiscConnection(void);
	//wxGxObject
	virtual wxString GetCategory(void){return wxString(_("Folder connection"));};
    virtual bool Destroy(void);
    virtual void Refresh(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void) const {return false;};
	virtual bool Rename(const wxString& NewName);
    virtual int GetStoreId(void) const {return m_nStoreId;};
//events
    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
#ifdef __UNIX__
    virtual void OnObjectAdded(wxGxCatalogEvent& event);
    virtual void OnObjectChanged(wxGxCatalogEvent& event);
#endif
protected:
    virtual void StartWatcher(void);
	virtual void LoadChildren(void);

protected:
    wxCriticalSection m_CritSect;
    int m_nStoreId;
    wxGxCatalog* m_pCatalog;
    long m_ConnectionPointCatalogCookie;
private:
    DECLARE_EVENT_TABLE()
};
