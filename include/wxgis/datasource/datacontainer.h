/******************************************************************************
 * Project:  wxGIS
 * Purpose:  datacontainer Folder, GeoDatabase & etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Barishnikov
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

#include "wxgis/datasource/dataset.h"

/** @class wxGISDataSource

    The DataSource class.

    @library{datasource}
*/
class WXDLLIMPEXP_GIS_DS wxGISDataSource :
    public wxGISDataset
{
    DECLARE_CLASS(wxGISDataSource)
public:
    wxGISDataSource(const CPLString &szPath);
    virtual ~wxGISDataSource(void);

    //wxGISDataset
    virtual void Close(void);
    virtual size_t GetSubsetsCount(void) const;
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxGISDataset* GetSubset(const wxString &sTableName);
    virtual wxString GetName(void) const;
    virtual bool Open(int bUpdate = TRUE);

    //wxGISDataset
    virtual bool Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel = NULL);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
    virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual char **GetFileList();
    //
    virtual OGRDataSource* const GetDataSourceRef(void) const { return m_poDS; };
protected:
    wxGISDataset* GetDatasetFromOGRLayer(const CPLString &sPath, OGRLayer* poLayer);
protected:
    OGRDataSource *m_poDS;
    wxString m_sDBName;
};
