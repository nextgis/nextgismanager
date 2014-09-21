/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection UI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/processing.h"
#include "wxgis/framework/applicationbase.h"

#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/dbschema_16.xpm"
#include "../../art/dbschema_48.xpm"
#include "../../art/layers_16.xpm"
#include "../../art/layer_16.xpm"
#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"
#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "../../art/properties.xpm"

#include "wx/busyinfo.h"
#include "wx/utils.h"
#include "wx/propdlg.h"
#include "wx/bookctrl.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalogui/remoteconndlgs.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteConnectionUI, wxGxRemoteConnection)

BEGIN_EVENT_TABLE(wxGxRemoteConnectionUI, wxGxRemoteConnection)
	EVT_THREAD(wxID_ANY, wxGxRemoteConnectionUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIconConn, const wxIcon &SmallIconConn, const wxIcon &LargeIconDisconn, const wxIcon &SmallIconDisconn) : wxGxRemoteConnection(oParent, soName, soPath), wxGxAutoRenamer()
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;

    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

wxIcon wxGxRemoteConnectionUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oLargeIconConn;
    else
        return m_oLargeIconDisconn;
}

wxIcon wxGxRemoteConnectionUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oSmallIconConn;
    else
        return m_oSmallIconDisconn;
}

void wxGxRemoteConnectionUI::EditProperties(wxWindow *parent)
{
	wxGISRemoteDBConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        Disconnect();
        //reread settings from connection file
        wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
        if(pDSet)
            pDSet->ReadConnectionFile();
	}
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    wxBusyCursor wait;
    //connect
	if(!Connect())
	{
		wxGISErrorMessageBox(_("Connect failed!"));
		return false;
	}

    return true;
}

bool wxGxRemoteConnectionUI::Connect(void)
{
    if(IsConnected())
        return true;
    bool bRes = true;
    //add pending item
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(NULL != pCat && m_PendingId == wxNOT_FOUND)
    {
        m_PendingId = pCat->AddPending(GetId());
        pCat->ObjectChanged(GetId());
    }
    //start thread to load schemes
    if (!CreateAndRunThread())
        return false;
    return bRes;
}


//thread to load remote DB tables
//before exit we assume that no tables exist
wxThread::ExitCode wxGxRemoteConnectionUI::Entry()
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(NULL != pDSet)
    {
        if(!pDSet->Open())
        {
            wxThreadEvent event( wxEVT_THREAD, EXIT_EVENT );
            wxQueueEvent( this, event.Clone() );
            return (wxThread::ExitCode)wxTHREAD_MISC_ERROR;
        }
    }
    wsDELETE(pDSet);

    if (!m_bChildrenLoaded)
    {
        LoadChildren(); // first load children
        wxThreadEvent event(wxEVT_THREAD, LOADED_EVENT);
        wxQueueEvent( this, event.Clone() );
    }

    wxThread::Sleep(m_nLongWait);

    wxThread::ExitCode eCode = wxGxRemoteConnection::Entry();
	
	wxThreadEvent event(wxEVT_THREAD, LOADED_EVENT);
	wxQueueEvent( this, event.Clone() );
		
	return eCode;
}

void wxGxRemoteConnectionUI::OnThreadFinished(wxThreadEvent& event)
{
    if (event.GetId() == LOADED_EVENT) //call after the LoadChildren exited
    {
        wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
        if (pCat && m_PendingId != wxNOT_FOUND)
        {
            pCat->RemovePending(m_PendingId);
            m_PendingId = wxNOT_FOUND;
            pCat->ObjectRefreshed(GetId());
            pCat->ObjectChanged(GetId());
        }
    }
    else if(event.GetId() == EXIT_EVENT)
    {
        wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
        if (pCat && m_PendingId != wxNOT_FOUND)
        {
            pCat->RemovePending(m_PendingId);
            m_PendingId = wxNOT_FOUND;
            pCat->ObjectRefreshed(GetId());
            pCat->ObjectChanged(GetId());
        }

        wxString sErr = wxString::Format(_("Operation '%s' failed!"), wxString(_("Connect")));
        wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
    }

    //else do nothing
}

wxGxRemoteDBSchema* wxGxRemoteConnectionUI::GetNewRemoteDBSchema(int nRemoteId, const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    if(!m_oLargeIconFeatureClass.IsOk())
        m_oLargeIconFeatureClass = wxIcon(pg_vec_48_xpm);
    if(!m_oSmallIconFeatureClass.IsOk())
        m_oSmallIconFeatureClass = wxIcon(pg_vec_16_xpm);
    if(!m_oLargeIconTable.IsOk())
        m_oLargeIconTable = wxIcon(table_pg_48_xpm);
    if(!m_oSmallIconTable.IsOk())
        m_oSmallIconTable = wxIcon(table_pg_16_xpm);
    if(!m_oLargeIconSchema.IsOk())
        m_oLargeIconSchema = wxIcon(dbschema_48_xpm);
    if(!m_oSmallIconSchema.IsOk())
        m_oSmallIconSchema = wxIcon(dbschema_16_xpm);

    return wxStaticCast(new wxGxRemoteDBSchemaUI(nRemoteId, pwxGISRemoteConn, this, sName, soPath, m_oLargeIconSchema, m_oSmallIconSchema, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass, m_oLargeIconTable, m_oSmallIconTable), wxGxRemoteDBSchema);
}

wxDragResult wxGxRemoteConnectionUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxRemoteConnectionUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
#ifdef wxGIS_HAVE_GEOPROCESSING

//    //1. fill the IGxDataset* array
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }

    wxVector<EXPORTED_DATASET> paVectorDatasets;
    wxVector<EXPORTED_DATASET> paRasterDatasets;
    wxVector<EXPORTED_DATASET> paTables;

    wxWindow* pWnd = dynamic_cast<wxWindow*>(GetApplication());
    wxGxObjectFilter* pFilter = new wxGxDatasetFilter(enumGISRasterDataset, enumRasterPostGIS);

    wxBusyCursor wait;

    for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
    {
        wxGxObject* pGxObject = pCatalog->FindGxObject(saGxObjectPaths[i]);
        if (NULL != pGxObject)
        {
            if (pGxObject->IsKindOf(wxCLASSINFO(wxGxRemoteDBSchema)))
            {
                //create schema
                wxBusyCursor wait;
                if (CreateSchema(pGxObject->GetName()))
                {
                    //copy input schema children
                    wxGxObjectContainer* pCont = wxDynamicCast(pGxObject, wxGxObjectContainer);
                    if (!pCont)
                        continue;

                    //lets load some tables
                    pCont->HasChildren();
                    wxSleep(5);

                    if(!pCont->HasChildren())
                    {
                        continue;
                    }

                    const wxGxObjectList lObj = pCont->GetChildren();
                    for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
                    {
                        wxGxObject *pGxObject = *it;
                        IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(pGxObject);
                        if (NULL != pGxDSet)
                        {
                            EXPORTED_DATASET data = { pGxObject->GetBaseName(), pGxDSet };
                            if (pGxDSet->GetType() == enumGISRasterDataset)
                                paRasterDatasets.push_back(data);
                            else if (pGxDSet->GetType() == enumGISFeatureDataset)
                                paVectorDatasets.push_back(data);
                            else if (pGxDSet->GetType() == enumGISTable)
                                paTables.push_back(data);
                        }
                    }

                    CPLString sDestPath = CPLFormFilename(GetPath(), pGxObject->GetName().ToUTF8(), "");

                    if (paRasterDatasets.size() == 1)
                    {
                        ExportSingleRasterDataset(pWnd, sDestPath, paRasterDatasets[0].sName, pFilter, paRasterDatasets[0].pDSet);
                    }
                    else if (paRasterDatasets.size() > 1)
                    {
                        ExportMultipleRasterDatasets(pWnd, sDestPath, pFilter, paRasterDatasets);
                    }
                    wxDELETE(pFilter);

                    pFilter = new wxGxFeatureDatasetFilter(enumVecPostGIS);
                    if (paVectorDatasets.size() == 1)
                    {
                        ExportSingleVectorDataset(pWnd, sDestPath, paVectorDatasets[0].sName, pFilter, paVectorDatasets[0].pDSet);
                    }
                    else if (paVectorDatasets.size() > 1)
                    {
                        ExportMultipleVectorDatasets(pWnd, sDestPath, pFilter, paVectorDatasets);
                    }
                    wxDELETE(pFilter);

                    pFilter = new wxGxTableFilter(enumTablePostgres);
                    if (paTables.size() == 1)
                    {
                        ExportSingleTable(pWnd, sDestPath, paTables[0].sName, pFilter, paTables[0].pDSet);
                    }
                    else if (paTables.size() > 1)
                    {
                        ExportMultipleTable(pWnd, sDestPath, pFilter, paTables);
                    }
                    wxDELETE(pFilter);
                }
                else
                {
                    //try to create another schema
					wxString sErr(_("Create schema failed!"));
                    wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));

                    continue;
                }
            }
        }
    }

#endif // wxGIS_HAVE_GEOPROCESSING

    return true;
}


//--------------------------------------------------------------
//class wxGxRemoteDBSchemaUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)

BEGIN_EVENT_TABLE(wxGxRemoteDBSchemaUI, wxGxRemoteDBSchema)
	EVT_THREAD(wxID_ANY, wxGxRemoteDBSchemaUI::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteDBSchemaUI::wxGxRemoteDBSchemaUI(int nRemoteId, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon, const wxIcon &LargeIconFeatureClass, const wxIcon &SmallIconFeatureClass, const wxIcon &LargeIconTable, const wxIcon &SmallIconTable) : wxGxRemoteDBSchema(nRemoteId, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
    m_oLargeIconFeatureClass = LargeIconFeatureClass;
    m_oSmallIconFeatureClass = SmallIconFeatureClass;
    m_oLargeIconTable = LargeIconTable;
    m_oSmallIconTable = SmallIconTable;

    m_PendingId = wxNOT_FOUND;
}

wxGxRemoteDBSchemaUI::~wxGxRemoteDBSchemaUI(void)
{
}

void wxGxRemoteDBSchemaUI::EditProperties(wxWindow *parent)
{
    //TODO: change permissions and properties
}

wxIcon wxGxRemoteDBSchemaUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxRemoteDBSchemaUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

bool wxGxRemoteDBSchemaUI::HasChildren(void)
{
    if(m_bChildrenLoaded)
        return wxGxObjectContainer::HasChildren();

    CreateAndRunThread();

    return wxGxObjectContainer::HasChildren();
}

wxGxObject* wxGxRemoteDBSchemaUI::GetNewTable(int nRemoteId, const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    if (sTableName.IsEmpty())
        return NULL;

    CPLString szPath(CPLFormFilename(GetPath(), sTableName.ToUTF8(), ""));

    switch (eType)
    {
    case enumGISFeatureDataset:
        return new wxGxPostGISFeatureDatasetUI(nRemoteId, GetName(), m_pwxGISRemoteConn, this, sTableName, szPath, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
    case enumGISRasterDataset:
        return NULL;
    case enumGISTable:
    default:
        return new wxGxPostGISTableUI(nRemoteId, GetName(), m_pwxGISRemoteConn, this, sTableName, szPath, m_oLargeIconTable, m_oSmallIconTable);
    };
}

wxDragResult wxGxRemoteDBSchemaUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxRemoteDBSchemaUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
#ifdef wxGIS_HAVE_GEOPROCESSING

    //1. fill the IGxDataset* array
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }

    wxVector<EXPORTED_DATASET> paVectorDatasets;
    wxVector<EXPORTED_DATASET> paRasterDatasets;
    wxVector<EXPORTED_DATASET> paTables;

    for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
    {
        wxGxObject* pGxObject = pCatalog->FindGxObject(saGxObjectPaths[i]);
        if (NULL != pGxObject)
        {
            if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
            {
                wxBusyCursor wait;
                wxGxDatasetContainer* pCont = wxDynamicCast(pGxObject, wxGxDatasetContainer);
                if (!pCont->HasChildren())
                    continue;
                const wxGxObjectList lObj = pCont->GetChildren();
                for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
                {
                    wxGxObject *pGxObject = *it;
                    IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(pGxObject);
                    if (NULL != pGxDSet)
                    {
                        wxString sName = CheckUniqTableName(pGxObject->GetBaseName());
                        EXPORTED_DATASET data = { sName, pGxDSet };
                        if (pGxDSet->GetType() == enumGISRasterDataset)
                            paRasterDatasets.push_back(data);
                        else if (pGxDSet->GetType() == enumGISFeatureDataset)
                            paVectorDatasets.push_back(data);
                        else if (pGxDSet->GetType() == enumGISTable)
                            paTables.push_back(data);
                    }
                }
            }
            else if ( pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
            {
                IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(pGxObject);
                if (NULL != pGxDSet)
                {
                    wxString sName = CheckUniqTableName(pGxObject->GetBaseName(), wxT("_"));
                    EXPORTED_DATASET data = { sName, pGxDSet };
                    if (pGxDSet->GetType() == enumGISRasterDataset)
                        paRasterDatasets.push_back(data);
                    else if (pGxDSet->GetType() == enumGISFeatureDataset)
                        paVectorDatasets.push_back(data);
                    else if (pGxDSet->GetType() == enumGISTable)
                        paTables.push_back(data);
                }
            }
        }
    }

    wxWindow* pWnd = dynamic_cast<wxWindow*>(GetApplication());
    //2. GxObject progress
    wxGxObjectFilter* pFilter = new wxGxDatasetFilter(enumGISRasterDataset, enumRasterPostGIS);
    if (paRasterDatasets.size() == 1)
    {
        ExportSingleRasterDataset(pWnd, GetPath(), paRasterDatasets[0].sName, pFilter, paRasterDatasets[0].pDSet);
    }
    else if (paRasterDatasets.size() > 1)
    {
        ExportMultipleRasterDatasets(pWnd, GetPath(), pFilter, paRasterDatasets);
    }
    wxDELETE(pFilter);

    pFilter = new wxGxFeatureDatasetFilter(enumVecPostGIS);
    if (paVectorDatasets.size() == 1)
    {
        ExportSingleVectorDataset(pWnd, GetPath(), paVectorDatasets[0].sName, pFilter, paVectorDatasets[0].pDSet);
    }
    else if (paVectorDatasets.size() > 1)
    {
        ExportMultipleVectorDatasets(pWnd, GetPath(), pFilter, paVectorDatasets);
    }
    wxDELETE(pFilter);

    pFilter = new wxGxTableFilter(enumTablePostgres);
    if (paTables.size() == 1)
    {
        ExportSingleTable(pWnd, GetPath(), paTables[0].sName, pFilter, paTables[0].pDSet);
    }
    else if (paTables.size() > 1)
    {
        ExportMultipleTable(pWnd, GetPath(), pFilter, paTables);
    }
    wxDELETE(pFilter);
	
	wxMilliSleep(500);
	OnGetUpdates();

#endif // wxGIS_HAVE_GEOPROCESSING

    return true;
}

bool wxGxRemoteDBSchemaUI::CreateAndRunThread(void)
{
    if (GetThread() == NULL)
    {
        if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    if (GetThread()->IsRunning())
        return true;

    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if (pCat)
    {
        m_PendingId = pCat->AddPending(GetId());
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }

    return true;
}

wxThread::ExitCode wxGxRemoteDBSchemaUI::Entry()
{
    if (!m_bChildrenLoaded)
    {
        LoadChildren();
        wxThreadEvent event(wxEVT_THREAD, LOADED_EVENT);
        wxQueueEvent(this, event.Clone());
    }

    wxThread::Sleep(m_nLongWait);

    return wxGxRemoteDBSchema::Entry();
}

void wxGxRemoteDBSchemaUI::OnThreadFinished(wxThreadEvent& event)
{
    if (event.GetId() == LOADED_EVENT)
    {
        wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
        if (pCat)
        {
            pCat->RemovePending(m_PendingId);
            m_PendingId = wxNOT_FOUND;
            pCat->ObjectRefreshed(GetId());
            pCat->ObjectChanged(GetId());
        }
    }
}

bool wxGxRemoteDBSchemaUI::Delete(void)
{
    if(m_sName == wxT("public"))
    {
	   wxGISErrorMessageBox(_("Deletion of the public scheme is not supported!"));
       return false;
    }
    return wxGxRemoteDBSchema::Delete();
}

#endif //wxGIS_USE_POSTGRES

//--------------------------------------------------------------
//class wxGxTMSWebServiceUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTMSWebServiceUI, wxGxTMSWebService)

wxGxTMSWebServiceUI::wxGxTMSWebServiceUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon, const wxIcon &icLargeIconDsbl, const wxIcon &icSmallIconDsbl) : wxGxTMSWebService(oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
    m_icLargeIconDsbl = icLargeIconDsbl;
    m_icSmallIconDsbl = icSmallIconDsbl;
}

wxGxTMSWebServiceUI::~wxGxTMSWebServiceUI(void)
{
}

wxIcon wxGxTMSWebServiceUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
    {
        return m_icLargeIcon;
    }
    else
    {
        return m_icLargeIconDsbl;
    }
}

wxIcon wxGxTMSWebServiceUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
    {
        return m_icSmallIcon;
    }
    else
    {
        return m_icSmallIconDsbl;
    }
}

void wxGxTMSWebServiceUI::EditProperties(wxWindow *parent)
{
	wxGISTMSConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        if(!m_pwxGISDataset)
            return;
        if(m_pwxGISDataset->IsOpened())
            m_pwxGISDataset->Close();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
	}
/*
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISRasterPropertyPage* RasterPropertyPage = new wxGISRasterPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(RasterPropertyPage, RasterPropertyPage->GetPageName());
	wxGISRasterDataset* pDset = wxDynamicCast(GetDataset(), wxGISRasterDataset);
	if(pDset)
	{
        if(!pDset->IsOpened())
            pDset->Open(true);
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());
        wsDELETE(pDset);
	}
    wxGISRasterHistogramPropertyPage* RasterHistogramPropertyPage = new wxGISRasterHistogramPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(RasterHistogramPropertyPage, RasterHistogramPropertyPage->GetPageName());

    //TODO: Additional page for virtual raster VRTSourcedDataset with sources files

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
	 */
}

