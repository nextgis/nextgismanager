/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgisdefs.h"

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"

class wxGxRemoteDBSchema;

/** \class wxGxRemoteConnection gxremoteconn.h
    \brief A Remote Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public IGxRemoteConnection
{
    enum
    {
        TIMER_ID = 1017
    };
    DECLARE_CLASS(wxGxRemoteConnection)
public:
	wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteConnection(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Database Connection"));};
    virtual void Refresh(void);
    //IGxRemoteConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual bool IsConnected(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(const wxString& NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanCopy(const CPLString &szDestPath){return true;};
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath){return CanCopy(szDestPath) & CanDelete();};
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	//virtual bool HasChildren(void);
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxRemoteConnection
    bool CreateSchema(const wxString& sSchemaName);
    wxString CheckUniqSchemaName(const wxString& sSchemaName, const wxString& sAdd = wxT(" "), int nCounter = 0);
protected:
	//wxGxRemoteConnection
	virtual void LoadChildren(void);
    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
    wxArrayString FillSchemaNames(wxGISTableCached* pInfoSchema);
    void DeleteSchema(const wxString& sSchemaName);
protected:
    //events
    virtual void OnTimer(wxTimerEvent & event);
protected:
    wxGISDataset* m_pwxGISDataset;
    bool m_bIsConnected;
    wxArrayString m_saSchemas;
    bool m_bHasGeom, m_bHasGeog, m_bHasRaster;
    wxTimer m_timer;
private:
    DECLARE_EVENT_TABLE()
};

/** \class wxGxRemoteDBSchema gxremoteconn.h
    \brief A Remote Database schema GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :	
	public wxGxObjectContainer,
    public IGxObjectEdit
{
    DECLARE_CLASS(wxGxRemoteDBSchema)
    enum
    {
        TIMER_ID = 1018
    };
public:
	wxGxRemoteDBSchema(bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteDBSchema(void);
	//wxGxObject
	virtual wxString GetCategory(void){return wxString(_("Remote Database Schema"));};
    virtual void Refresh(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
    //IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void);
	virtual bool Rename(const wxString& NewName);
    virtual bool CanRename(void);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
    //TODO: check posibility to release copy and move db schema
	virtual bool CanCopy(const CPLString &szDestPath){return false;};//The table schema cannot be copied 
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel){return false;};
	virtual bool CanMove(const CPLString &szDestPath){return false;};//The table schema cannot be moved

protected:
    //wxGxRemoteDBSchema
    virtual void LoadChildren(void);
    virtual wxArrayString FillTableNames(void);
    virtual wxGxObject* AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType);
    virtual void DeleteTable(const wxString& sSchemaName);
protected:
    //events
    virtual void OnTimer(wxTimerEvent & event);
protected:
    wxGISPostgresDataSource* m_pwxGISRemoteConn;
    bool m_bChildrenLoaded;
    wxCriticalSection m_CritSect;
    bool m_bHasGeom, m_bHasGeog, m_bHasRaster;
    wxArrayString m_saTables;
    wxTimer m_timer;
private:
    DECLARE_EVENT_TABLE()
};

#endif //wxGIS_USE_POSTGRES


/** \class wxGxTMSWebService gxremoteconn.h
    \brief A TMS Web Service GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxTMSWebService :
	public wxGxRasterDataset
{
    DECLARE_CLASS(wxGxRemoteConnection)
public:
	wxGxTMSWebService(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxTMSWebService(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Web service"));};
    //wxGxDataset
    virtual void FillMetadata(bool bForce = false);
};