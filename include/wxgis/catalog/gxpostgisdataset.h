/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/catalog/gxdataset.h"

/** @class wxGxPostGISTableDataset
 * 
 * A PostGIS Table Dataset GxObject.
 * 
 * @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxPostGISTableDataset :
	public wxGxTableDataset
{
    DECLARE_CLASS(wxGxPostGISTableDataset)
public:
	wxGxPostGISTableDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxPostGISTableDataset(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("PostGIS Table"));};
    //wxGxDataset
    virtual void FillMetadata(bool bForce);
    //IGxDatasetEdit
    virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool CanMove(const CPLString &szDestPath);
    virtual bool Rename(const wxString &sNewName);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxString m_sSchemaName;
    wxString m_sFullyQualifiedName;
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
};

/** @class wxGxPostGISFeatureDataset
 *  
 *  A PostGIS Feature Dataset GxObject.
 * 
 *  @library{catalog}
 */

class WXDLLIMPEXP_GIS_CLT wxGxPostGISFeatureDataset :
	public wxGxFeatureDataset
{
    DECLARE_CLASS(wxGxPostGISFeatureDataset)
public:
	wxGxPostGISFeatureDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxPostGISFeatureDataset(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("PostGIS Feature Dataset"));};
    //wxGxDataset
    virtual void FillMetadata(bool bForce);
    //IGxDatasetEdit
    virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
    virtual bool CanMove(const CPLString &szDestPath);
    virtual bool Rename(const wxString &sNewName);
    virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
    virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
protected:
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
    wxString m_sSchemaName;
    wxString m_sFullyQualifiedName;
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
};

#endif //wxGIS_USE_POSTGRES