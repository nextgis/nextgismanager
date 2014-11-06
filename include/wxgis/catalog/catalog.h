/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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

#include "wxgis/core/config.h"
#include "wxgis/datasource/dataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"

#undef LT_OBJDIR
#include "gdal_priv.h"
#include "gdal.h"

#include <wx/filename.h>

#define NOTFIRESELID    1010
#define CONNDIR wxT("connections")

enum wxGISEnumSaveObjectResults
{
	enumGISSaveObjectNone = 0x0000,
	enumGISSaveObjectAccept = 0x0001,
	enumGISSaveObjectExists = 0x0002,
	enumGISSaveObjectDeny = 0x0004
};

/** @class IGxObjectEdit

    A GxObject edit interface.

    @library{catalog}
*/

class IGxObjectEdit
{
public:
	virtual ~IGxObjectEdit(void){};
	virtual bool Delete(void){return false;};
    virtual bool CanDelete(void){ return false; };
	virtual bool Rename(const wxString& NewName){return false;};
    virtual bool CanRename(void){ return false; };
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
    virtual bool CanCopy(const CPLString &szDestPath){ return false; };
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
	virtual bool CanMove(const CPLString &szDestPath){return false;};
};

/** @class IGxRootObjectProperties
    
    An Interface class for Root GxObject.

    @library{catalog}
*/

class IGxRootObjectProperties
{
public:
	virtual ~IGxRootObjectProperties(void){};
	virtual void Init(wxXmlNode* const pConfigNode) = 0;
	virtual void Serialize(wxXmlNode* const pConfigNode) = 0;
};

/** @class IGxRemoteConnection

    A GxObject remote connection interface.

    @library{catalog}
*/

class IGxRemoteConnection
{
public:
	virtual ~IGxRemoteConnection(void){};
	virtual bool Connect(void) = 0;
	virtual bool Disconnect(void) = 0;
    virtual bool IsConnected(void) = 0;
};

/** @class IGxDataset

    A GxObject dataset interface.

    @library{catalog}
*/

class IGxDataset
{
public:
	virtual ~IGxDataset(void){};
	virtual wxGISDataset* const GetDataset(bool bCached = true, ITrackCancel* const pTrackCancel = NULL) = 0;
	virtual wxGISEnumDatasetType GetType(void) const = 0;
	virtual int GetSubType(void) const = 0;
    virtual wxULongLong GetSize(void) const = 0;
    virtual wxDateTime GetModificationDate(void) const = 0;
    virtual void FillMetadata(bool bForce = false) = 0;
    virtual bool IsMetadataFilled() const = 0;
};

/** @class IGxObjectNoFilter

    An Interface class indicates show object on all views (e.g. Open/Save dialog).

    @library{catalog}
*/

class IGxObjectNoFilter
{
public:
    virtual ~IGxObjectNoFilter(void){};
};

/** @class IGxObjectNotifier

    An Interface class to notify parant GxObject that it need get updates or something else

    @library{catalog}
*/

class IGxObjectNotifier
{
public:
    virtual ~IGxObjectNotifier(void){};
	virtual void OnGetUpdates(int nDelay = 50) = 0;
};

/*
class IGxFile
{
public:
	virtual ~IGxFile(void){};
	//virtual bool Open(void) = 0;
	//virtual bool Close(bool bSaveEdits) = 0;
	//virtual void Edit(void) = 0;
	//virtual void New(void) = 0;
	//virtual bool Save(void) = 0;
};

*/


