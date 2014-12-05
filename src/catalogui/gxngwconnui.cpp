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

#include "wxgis/catalogui/gxngwconnui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/processing.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/json/jsonwriter.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/carto/mapbitmap.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/rasterlayer.h"

#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/layers_16.xpm"
#include "../../art/layer_16.xpm"
#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"
#include "../../art/ngw_layer_16.xpm"
#include "../../art/ngw_layer_48.xpm"
#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"
#include "../../art/rdb_disconn_16.xpm"
#include "../../art/rdb_disconn_48.xpm"
#include "../../art/raster_bmp16.xpm"
#include "../../art/raster_bmp48.xpm"
#include "../../art/properties.xpm"

#include "../../art/package_16.xpm"
#include "../../art/package_48.xpm"
#include "../../art/raster_tif16.xpm"
#include "../../art/raster_tif48.xpm"
#include "../../art/raster_16.xpm"
#include "../../art/raster_48.xpm"
#include "../../art/raster_img16.xpm"
#include "../../art/raster_img48.xpm"
#include "../../art/raster_png16.xpm"
#include "../../art/raster_png48.xpm"
#include "../../art/raster_saga16.xpm"
#include "../../art/raster_saga48.xpm"
#include "../../art/raster_gif16.xpm"
#include "../../art/raster_gif48.xpm"
#include "../../art/raster_til16.xpm"
#include "../../art/raster_til48.xpm"
#include "../../art/raster_vrt16.xpm"
#include "../../art/raster_vrt48.xpm"
#include "../../art/shp_dset_16.xpm"
#include "../../art/shp_dset_48.xpm"
#include "../../art/mi_dset_16.xpm"
#include "../../art/mi_dset_48.xpm"
#include "../../art/md_dset_16.xpm"
#include "../../art/md_dset_48.xpm"
#include "../../art/sxf_dset_16.xpm"
#include "../../art/sxf_dset_48.xpm"
#include "../../art/dxf_dset_16.xpm"
#include "../../art/dxf_dset_48.xpm"
#include "../../art/kml_dset_16.xpm"
#include "../../art/kml_dset_48.xpm"
#include "../../art/kmz_dset_16.xpm"
#include "../../art/kmz_dset_48.xpm"
#include "../../art/gml_dset_16.xpm"
#include "../../art/gml_dset_48.xpm"
#include "../../art/json_dset_16.xpm"
#include "../../art/json_dset_48.xpm"
#include "../../art/csv_16.xpm"
#include "../../art/csv_48.xpm"
#include "../../art/localc_16.xpm"
#include "../../art/localc_48.xpm"
#include "../../art/table_dbf_16.xpm"
#include "../../art/table_dbf_48.xpm"


//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"
#include "wxgis/catalogui/createremotedlgs.h"
#include "wxgis/catalog/gxpostgisdataset.h"

#include "wx/busyinfo.h"
#include "wx/utils.h"
#include "wx/propdlg.h"
#include "wx/bookctrl.h"

#ifdef wxGIS_HAVE_GEOPROCESSING
	#include "wxgis/geoprocessing/gpvector.h"
    #include "wxgis/geoprocessing/gpraster.h"
#endif 	//wxGIS_HAVE_GEOPROCESSING

#ifdef wxGIS_USE_CURL


#include "wxgis/catalogui/remoteconndlgs.h"

//--------------------------------------------------------------
//class wxGxNGWServiceUI
//--------------------------------------------------------------

class wxGxNGWLayerUI;
IMPLEMENT_CLASS(wxGxNGWServiceUI, wxGxNGWService)

wxGxNGWServiceUI::wxGxNGWServiceUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon, const wxIcon &icLargeIconDsbl, const wxIcon &icSmallIconDsbl, const wxIcon &icLargeIconGuest, const wxIcon &icSmallIconGuest) : wxGxNGWService(oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
    m_icLargeIconDsbl = icLargeIconDsbl;
    m_icSmallIconDsbl = icSmallIconDsbl;
    m_icLargeIconGuest = icLargeIconGuest;
    m_icSmallIconGuest = icSmallIconGuest;
}

wxGxNGWServiceUI::~wxGxNGWServiceUI(void)
{
}

wxIcon wxGxNGWServiceUI::GetLargeImage(void)
{
    if (IsConnected())
    {
		if(m_bIsAuthorized)
		{
			return m_icLargeIcon;
		}
		else
		{
			return m_icLargeIconGuest;
		}
    }
    else
    {
        return m_icLargeIconDsbl;
    }
}

wxIcon wxGxNGWServiceUI::GetSmallImage(void)
{
    if (IsConnected())
    {
		if(m_bIsAuthorized)
		{
			return m_icSmallIcon;			
		}
		else
		{
			return m_icSmallIconGuest;	
		}
    }
    else
    {
        return m_icSmallIconDsbl;
    }
}

void wxGxNGWServiceUI::EditProperties(wxWindow *parent)
{
	wxGISNGWConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        Disconnect();
        //reread settings from connection file
        ReadConnectionFile();
	}	
}

bool wxGxNGWServiceUI::Invoke(wxWindow* pParentWnd)
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

void wxGxNGWServiceUI::LoadChildren(void)
{
    if (m_bChildrenLoaded || !m_bIsConnected)
        return;

    wxGxNGWRootResourceUI * pGxNGWRootResourceUI = new wxGxNGWRootResourceUI(this, this, _("Resources"), CPLString(m_sURL.ToUTF8()), wxNullIcon, wxIcon(layers_16_xpm));
	pGxNGWRootResourceUI->FillPermissions();
/*    if(m_bIsAuthorized)
        new wxGxNGWRootUI(this, _("Administration"), CPLString(m_sURL.ToUTF8()), wxNullIcon, wxNullIcon, wxNullIcon, wxNullIcon);
        */
    m_bChildrenLoaded = true;
}

//--------------------------------------------------------------
//class wxGxNGWRootResourceUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWRootResourceUI, wxGxNGWResourceGroupUI)

wxGxNGWRootResourceUI::wxGxNGWRootResourceUI(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWResourceGroupUI(pService, wxJSONValue(), oParent, soName, soPath, icLargeIcon, icSmallIcon)
{
    m_nRemoteId = 0;
    m_sName = wxString(_("Resources"));
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), 0);
	m_sPath = CPLString(sURL.ToUTF8());
}

wxGxNGWRootResourceUI::~wxGxNGWRootResourceUI(void)
{
}

wxIcon wxGxNGWRootResourceUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWRootResourceUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

//--------------------------------------------------------------
//class wxGxNGWResourceGroupUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWResourceGroupUI, wxGxNGWResourceGroup)

wxGxNGWResourceGroupUI::wxGxNGWResourceGroupUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWResourceGroup(pService, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWResourceGroupUI::~wxGxNGWResourceGroupUI(void)
{
}

wxIcon wxGxNGWResourceGroupUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWResourceGroupUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

wxGxObject* wxGxNGWResourceGroupUI::AddResource(const wxJSONValue &Data)
{
    wxGISEnumNGWResourcesType eType = GetType(Data);
	wxGxObject* pReturnObj(NULL);
	wxString sName = Data["resource"]["display_name"].AsString();
	
    switch(eType)
    {
    case enumNGWResourceTypeResourceGroup:
		if(!m_icFolderLargeIcon.IsOk())
			m_icFolderLargeIcon = wxIcon(folder_arch_48_xpm);
 		if(!m_icFolderSmallIcon.IsOk())
			m_icFolderSmallIcon = wxIcon(folder_arch_16_xpm);
        pReturnObj = wxDynamicCast(new wxGxNGWResourceGroupUI(m_pService, Data, this, wxEmptyString, m_sPath, m_icFolderLargeIcon, m_icFolderSmallIcon), wxGxObject);
        break;
	case enumNGWResourceTypePostgisLayer:
		if(!m_icPGLayerLargeIcon.IsOk())
			m_icPGLayerLargeIcon = wxIcon(pg_vec_48_xpm);
 		if(!m_icPGLayerSmallIcon.IsOk())
			m_icPGLayerSmallIcon = wxIcon(pg_vec_16_xpm);
        if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayerUI(m_pService, enumNGWResourceTypePostgisLayer, Data, this, wxEmptyString, m_sPath, m_icPGLayerLargeIcon, m_icPGLayerSmallIcon), wxGxObject);
		break;
	case enumNGWResourceTypePostgisConnection:
		if(!m_icPGConnLargeIcon.IsOk())
			m_icPGConnLargeIcon = wxIcon(rdb_conn_48_xpm);
 		if(!m_icPGConnSmallIcon.IsOk())
			m_icPGConnSmallIcon = wxIcon(rdb_conn_16_xpm);
		if(!m_icPGDisConnLargeIcon.IsOk())
			m_icPGDisConnLargeIcon = wxIcon(rdb_disconn_48_xpm);
 		if(!m_icPGDisConnSmallIcon.IsOk())
			m_icPGDisConnSmallIcon = wxIcon(rdb_disconn_16_xpm);
		if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWPostGISConnectionUI(m_pService, Data, this, wxEmptyString, m_sPath, m_icPGConnLargeIcon, m_icPGConnSmallIcon, m_icPGDisConnLargeIcon, m_icPGDisConnSmallIcon), wxGxObject);
		break;
	case enumNGWResourceTypeWMSServerService:
		break;
	case enumNGWResourceTypeBaseLayers:
		break;
	case enumNGWResourceTypeWebMap:
		break;
	case enumNGWResourceTypeWFSServerService:
		break;
	case enumNGWResourceTypeVectorLayer:
		if(!m_icNGWLayerLargeIcon.IsOk())
			m_icNGWLayerLargeIcon = wxIcon(ngw_layer_48_xpm);
 		if(!m_icNGWLayerSmallIcon.IsOk())
			m_icNGWLayerSmallIcon = wxIcon(ngw_layer_16_xpm);
        if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayerUI(m_pService, enumNGWResourceTypeVectorLayer, Data, this, wxEmptyString, m_sPath, m_icNGWLayerLargeIcon, m_icNGWLayerSmallIcon), wxGxObject);
		break;
	case enumNGWResourceTypeRasterLayer:
		if(!m_icNGWRasterLargeIcon.IsOk())
			m_icNGWRasterLargeIcon = wxIcon(raster_bmp48_xpm);
 		if(!m_icNGWRasterSmallIcon.IsOk())
			m_icNGWRasterSmallIcon = wxIcon(raster_bmp16_xpm);
		if(m_bHasWMS)
			pReturnObj = wxDynamicCast(new wxGxNGWRasterUI(m_pService, Data, this, wxEmptyString, m_sPath, m_icNGWRasterLargeIcon, m_icNGWRasterSmallIcon), wxGxObject);
		break;	
    case enumNGWResourceTypeFileSet:
	{
		wxIcon icLargeIcon, icSmallIcon;
#define SET_ICONS(n, l, s) { \
			if(!m_stmIconSet[n].icLargeIcon.IsOk()) \
				m_stmIconSet[n].icLargeIcon = wxIcon(l); \
			if(!m_stmIconSet[n].icSmallIcon.IsOk())	\
				m_stmIconSet[n].icSmallIcon = wxIcon(s); \
			icLargeIcon = m_stmIconSet[n].icLargeIcon; \
			icSmallIcon = m_stmIconSet[n].icSmallIcon; }
		
		if(sName.MakeLower().EndsWith(wxT("shp")))
			SET_ICONS("shp", shp_dset_48_xpm, shp_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("tab")))
			SET_ICONS("tab", mi_dset_48_xpm, mi_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("mif")))
			SET_ICONS("mif", md_dset_48_xpm, md_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("kml")))
			SET_ICONS("kml", kml_dset_48_xpm, kml_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("kmz")))
			SET_ICONS("kmz", kmz_dset_48_xpm, kmz_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("dxf")))
			SET_ICONS("dxf", dxf_dset_48_xpm, dxf_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("gml")))
			SET_ICONS("gml", gml_dset_48_xpm, gml_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("geojson")))
			SET_ICONS("geojson", json_dset_48_xpm, json_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("sxf")))
			SET_ICONS("sxf", sxf_dset_48_xpm, sxf_dset_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("000")))
			SET_ICONS("package", package_48_xpm, package_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("csv")))
			SET_ICONS("csv", csv_48_xpm, csv_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("ods")))
			SET_ICONS("xlsx", localc_48_xpm, localc_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("dbf")))
			SET_ICONS("dbf", table_dbf_48_xpm, table_dbf_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("xls")))
			SET_ICONS("xlsx", localc_48_xpm, localc_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("xlsx")))
			SET_ICONS("xlsx", localc_48_xpm, localc_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("bmp")))
			SET_ICONS("bmp", raster_bmp48_xpm, raster_bmp16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("tif")) || sName.MakeLower().EndsWith(wxT("tiff")))
			SET_ICONS("tif", raster_tif48_xpm, raster_tif16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("jpg")))
			SET_ICONS("jpg", raster_48_xpm, raster_16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("img")))
			SET_ICONS("img", raster_img48_xpm, raster_img16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("png")))
			SET_ICONS("png", raster_png48_xpm, raster_png16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("gif")))
			SET_ICONS("gif", raster_gif48_xpm, raster_gif16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("sdat")))
			SET_ICONS("sdat", raster_saga48_xpm, raster_saga16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("til")))
			SET_ICONS("til", raster_til48_xpm, raster_til16_xpm)
		else if(sName.MakeLower().EndsWith(wxT("vrt")))
			SET_ICONS("vrt", raster_vrt48_xpm, raster_vrt16_xpm)
		else 
			SET_ICONS("package", package_48_xpm, package_16_xpm)
			
		pReturnObj = wxDynamicCast(new wxGxNGWFileSetUI(m_pService, Data, this, wxEmptyString, m_sPath, icLargeIcon, icSmallIcon), wxGxObject);
	}
        break;
    }
	
	wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pReturnObj);
	if(pGxNGWResource)
		pGxNGWResource->FillPermissions();
	
	return pReturnObj;	
}

bool wxGxNGWResourceGroupUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{	
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }
	
	if(saGxObjectPaths.IsEmpty())
	{
		return false;
	}
	
	bool bIsNGWResource = false;
	bool bIsSameService = false;
	//check the first element type
	wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pCatalog->FindGxObject(saGxObjectPaths[0]));
	if(NULL != pGxNGWResource)
	{
		bIsNGWResource = true;
		//check if the same NGW by service address
		wxGxNGWService* pNGWServiceOther = pGxNGWResource->GetNGWService();
		wxGxNGWService* pNGWServiceThis = GetNGWService();
		bIsSameService = NULL != pNGWServiceThis && NULL != pNGWServiceOther && pNGWServiceThis->GetURL().IsSameAs(pNGWServiceOther->GetURL(), false);
	}
	
	//1. if saGxObjectPaths from the same NGW service - move
    wxString sOper(bMove == true ? _("Move") : _("Copy"));
    wxString sTitle = wxString::Format(_("%s %ld objects (files)"), sOper.c_str(), saGxObjectPaths.GetCount());
    wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());

    wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), saGxObjectPaths.GetCount(), pParentWnd);

	if(bIsNGWResource)
	{		
		ProgressDlg.ShowProgress(true);
		//bool bCopyAsk = true;
		if(bIsSameService && bMove)
		{
			for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
			{
				wxString sMessage = wxString::Format(_("%s %ld object (resource) from %ld"), sOper.c_str(), i + 1, saGxObjectPaths.GetCount());
				//ProgressDlg.SetTitle(sMessage);
				ProgressDlg.PutMessage(sMessage);
				if(!ProgressDlg.Continue())
					break;
					
				wxGxNGWResource* pGxNGWResource = dynamic_cast<wxGxNGWResource*>(pCatalog->FindGxObject(saGxObjectPaths[i]));
				bool bRes = pGxNGWResource->MoveResource(GetRemoteId());
				//report error
				if(!bRes)
				{
					wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Copy"));  
					sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
					wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, &ProgressDlg);
				}
				else
				{
					wxGxObject* pObj = dynamic_cast<wxGxObject*>(pGxNGWResource);
					if(NULL != pObj)
					{
						IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(pObj->GetParent());
						if(pNotify)
						{	
							pNotify->OnGetUpdates();
						}						
					}		
				}
				ProgressDlg.SetValue(i);
			}
						
			//report error if any
			ShowMessageDialog(pParentWnd, ProgressDlg.GetWarnings());

			//notify this on updates
			OnGetUpdates();
				
			return true;
		}
		else
		{
			// copy from local or remote NGW
			for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
			{
				wxString sMessage = wxString::Format(_("%s %ld object (resource) from %ld"), sOper.c_str(), i + 1, saGxObjectPaths.GetCount());
				ProgressDlg.PutMessage(sMessage);
				if(!ProgressDlg.Continue())
					break;
					
				wxGxObject* pGxObj = pCatalog->FindGxObject(saGxObjectPaths[i]);
				IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObj);				
				if(pGxObjectEdit && pGxObjectEdit->CanCopy(GetPath()))
				{
					if(!pGxObjectEdit->Copy(GetPath(), &ProgressDlg))
					{
						wxGISErrorMessageBox(wxString::Format(_("%s failed. Path: %s"), _("Copy"), pGxObj->GetFullName()));
						return false;
					}
				}
				ProgressDlg.SetValue(i);
			}
			
			ShowMessageDialog(pParentWnd, ProgressDlg.GetWarnings());
			
			//notify this on updates
			OnGetUpdates();
		}
	}
	else
	{
		//check if drop object from postgis connection
		wxGxNGWResource* pGxNGWPostGISConnection = NULL;
		wxGxObject* pGxObject = pCatalog->FindGxObject(saGxObjectPaths[0]);
		if(pGxObject)
		{
			wxGxObject* pGxParentObject;
			while((pGxParentObject = pGxObject->GetParent()) != NULL)
			{
				pGxNGWPostGISConnection = dynamic_cast<wxGxNGWResource*>(pGxParentObject);
				if(NULL != pGxNGWPostGISConnection)
				{
					break;
				}
				
				pGxObject = pGxParentObject;
			}
		}
		
		//create PostGIS Layer
		if(NULL != pGxNGWPostGISConnection)
		{
			if(!m_pService->IsTypeSupported(enumNGWResourceTypePostgisLayer))
			{
				ProgressDlg.PutMessage(_("The type PostgisLayer is unsupported"), wxNOT_FOUND, enumGISMessageWarning);
			}
			else
			{
				ProgressDlg.ShowProgress(true);
				//don't forget rename id field in created layer or something else
				for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
				{
					wxString sMessage = wxString::Format(_("%s %ld object (resource) from %ld"), sOper.c_str(), i + 1, saGxObjectPaths.GetCount());
					//ProgressDlg.SetTitle(sMessage);
					ProgressDlg.PutMessage(sMessage);
					if(!ProgressDlg.Continue())
						break;
						
					wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = wxDynamicCast(pCatalog->FindGxObject(saGxObjectPaths[i]), wxGxPostGISFeatureDataset);
					if (NULL != pGxPostGISFeatureDataset)
					{
						wxString sName = pGxPostGISFeatureDataset->GetName();
						int nConnId = pGxNGWPostGISConnection->GetRemoteId();
						wxString sTable = pGxPostGISFeatureDataset->GetTableName();
						wxString sSchema = pGxPostGISFeatureDataset->GetTableSchemaName();
						wxGISFeatureDataset* pDataset = wxDynamicCast(pGxPostGISFeatureDataset->GetDataset(false), wxGISFeatureDataset);
						if(NULL == pDataset)
						{
							wxGISErrorMessageBox(_("Failed to get PostGIS Dataset"));
							continue;
						}
						wxString sFid = pDataset->GetFIDColumn();
						wxString sGeom = pDataset->GetGeometryColumn();
						
						if(!CreatePostGISLayer(sName, nConnId, sTable, sSchema, sFid, sGeom))
						{
							wxGISErrorMessageBox(_("Failed to create PostGIS Layer"), wxString::FromUTF8(CPLGetLastErrorMsg()));
						}
					}
					ProgressDlg.SetValue(i);				
				}
			}
			
			ShowMessageDialog(pParentWnd, ProgressDlg.GetWarnings());
			
			OnGetUpdates();
		}
		else
		{
			//upload other vector and raster DS
			if(!m_pService->IsTypeSupported(enumNGWResourceTypeFileSet))
			{
				ProgressDlg.PutMessage(_("The type FileBucket is unsupported"), wxNOT_FOUND, enumGISMessageWarning);
			}
			else
			{				
				wxVector<wxGxObject*> paDatasets;
				for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
				{
					wxBusyCursor wait;
					wxGxObject* pGxObject = pCatalog->FindGxObject(saGxObjectPaths[i]);
					if (NULL != pGxObject)
					{
						//add file based datasets
						IGxDataset *pDataset = dynamic_cast<IGxDataset*>(pGxObject);
						//add archives
						wxGxArchive *pArchive = dynamic_cast<wxGxArchive*>(pGxObject);
						//add wxGxFile
						wxGxFile *pFile = dynamic_cast<wxGxFile*>(pGxObject);
						//TODO: wxGxQGISProjFile etc.
						if (pDataset && IsFileDataset(pDataset->GetType(), pDataset->GetSubType()))
						{						
							paDatasets.push_back(pGxObject);
						}
						else if(pArchive)
						{
							paDatasets.push_back(pGxObject);
						}
						else if(pFile)
						{
							paDatasets.push_back(pGxObject);
						}
					}
				}
				
				size_t nCount = paDatasets.size();
				ProgressDlg.SetTitle(_("Upload selected items"));
				ProgressDlg.ShowProgress(true);
				for ( size_t j = 0; j < nCount; ++j ) 
				{				
					ProgressDlg.SetRange(nCount);
					ProgressDlg.SetValue(j);
					if(!ProgressDlg.Continue())
					{
						return false;
					}
					
					IGxDataset* pGxDset = dynamic_cast<IGxDataset*>(paDatasets[j]);
					wxGxArchive *pArchive = dynamic_cast<wxGxArchive*>(paDatasets[j]);
					wxGxFile *pFile = dynamic_cast<wxGxFile*>(paDatasets[j]);
					if(pGxDset)
					{
						wxGISDataset* pDSet = pGxDset->GetDataset(false, &ProgressDlg);
						if(pDSet)
						{
							int nPreviewXSize = 640;
							int nPreviewYSize = 640;
							wxGISAppConfig oConfig = GetConfig();
							if(oConfig.IsOk())
							{
								wxString sAppName = GetApplication()->GetAppName();
								nPreviewXSize = oConfig.ReadInt(enumGISHKCU, sAppName + wxT("/ngw/preview_x_size"), nPreviewXSize);
								nPreviewYSize = oConfig.ReadInt(enumGISHKCU, sAppName + wxT("/ngw/preview_y_size"), nPreviewYSize);
							}
							
							//create raster preview
							CPLString osTmpPath = CPLGenerateTempFilename( "ngw" );
							CPLString osPreviewPath = CPLFormFilename(CPLGetPath(osTmpPath), PREVIEW_FILE_NAME, PREVIEW_FILE_NAME_EXT);
							wxGISMapBitmap bmp(nPreviewXSize, nPreviewYSize); 
							bmp.SetTrackCancel(&ProgressDlg);
							wxVector<wxGISLayer*> paLayers;
							
							switch(pDSet->GetType())
							{
								case enumGISFeatureDataset:
									paLayers.push_back(bmp.GetLayerFromDataset(pDSet));
									while(pDSet->IsCaching())
									{
										wxSleep(1);
									}	
									break;
								case enumGISRasterDataset:
									paLayers.push_back(bmp.GetLayerFromDataset(pDSet));
									break;
								case enumGISContainer:
									//iterate on datasets of the container
									{
										wxGxDatasetContainer* pGxDatasetCont = dynamic_cast<wxGxDatasetContainer*>(pGxDset);
										if(pGxDatasetCont && pGxDatasetCont->HasChildren(true))
										{
											wxGxObjectList ObjectList = pGxDatasetCont->GetChildren();
											wxGxObjectList::iterator iter;
											for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
											{
												wxGxObject *current = *iter;
												wxGxDataset* pGxDataset = wxDynamicCast(current, wxGxDataset);
												if(pGxDataset)
												{
													wxGISDataset* pwxGISDataset = pGxDataset->GetDataset(true, &ProgressDlg);	
													if(pwxGISDataset)
													{
														wxGISLayer* pLayer = bmp.GetLayerFromDataset(pwxGISDataset);
														if(pLayer)
														{
															paLayers.push_back(pLayer);
														}
														while(pwxGISDataset->IsCaching())
														{
															wxSleep(1);
														}
														wsDELETE(pwxGISDataset);
													}
												}
											}
										}
									}
								break;
							};
							
							for (size_t k = 0; k < paLayers.size(); ++k)
							{
								if (paLayers[k])
								{
									if (paLayers[k]->IsValid())
									{
										bmp.AddLayer(paLayers[k]);
									}
									else
									{
										wxDELETE(paLayers[k]);
									}
								}
							}
							
							
					
							wxArrayString paths;
							char** papszFileList = pDSet->GetFileList();
							papszFileList = CSLAddString(papszFileList, pDSet->GetPath());
							
							//add it to paths list
							bmp.SetFullExtent();
							if (bmp.SaveAsBitmap(osPreviewPath, enumRasterPng, NULL, false))
								papszFileList = CSLAddString(papszFileList, osPreviewPath);
							
							for (int k = 0; papszFileList[k] != NULL; ++k)
							{
								wxString sPath = wxString::FromUTF8(papszFileList[k]);
								if(sPath.StartsWith(wxT("/vsi")))
								{
									ProgressDlg.PutMessage(wxString::Format(_("The archived file '%s' cannot be added to file set"), sPath.c_str()), wxNOT_FOUND, enumGISMessageWarning);
								}
								else
								{
									paths.Add(sPath);
								}
							}
							CSLDestroy(papszFileList);
							CreateFileBucket(paDatasets[j]->GetName(), paths, wxGxNGWResource::MakeMetadata(pDSet), &ProgressDlg);
							
							wsDELETE(pDSet);
						}
					}
					else if(pArchive)
					{
						wxArrayString paths;
						paths.Add(wxString::FromUTF8(pArchive->GetRealPath()));
						CreateFileBucket(paDatasets[j]->GetName(), paths, wxJSONValue(wxJSONTYPE_INVALID), &ProgressDlg);					
					}
					else if(pFile)
					{
						wxArrayString paths;
						paths.Add(wxString::FromUTF8(pFile->GetPath()));
						CreateFileBucket(paDatasets[j]->GetName(), paths, wxJSONValue(wxJSONTYPE_INVALID), &ProgressDlg);
					}				
				}
			}
			
			ShowMessageDialog(pParentWnd, ProgressDlg.GetWarnings());		
			OnGetUpdates();
			return true;
		}
	}
	return false;

	//2. if saGxObjectPaths from the same NGW service and copy or from the other NGW service - copy
	//3. if saGxObjectPaths is feature class or folder with feature class - export
}

wxArrayString wxGxNGWResourceGroupUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISNGWResourcePropertyPage");
	out.Add("wxGISNGWMetaPropertyPage");
	return out;
}

bool wxGxNGWResourceGroupUI::HasPropertyPages(void) const
{
	return true;
}

bool wxGxNGWResourceGroupUI::CreateVectorLayer(const wxString &sName, wxGISDataset * const pInputDataset, OGRwkbGeometryType eFilterGeomType, ITrackCancel* const pTrackCancel)
{
#ifdef wxGIS_HAVE_GEOPROCESSING	
	wxGISFeatureDataset* pInputFeatureDataset = dynamic_cast<wxGISFeatureDataset*>(pInputDataset); 
	if(NULL == pInputFeatureDataset)
	{
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Dataset type is not supported. Feature Class expected."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
	}
	
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
	{
		if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("cURL initialize failed."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
	}
	
	//1. create temp shp file
	if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Prepare ESRI Shape file"), wxNOT_FOUND, enumGISMessageTitle);
    }
	
	const wxGISSpatialReference SrcSpaRef = pInputFeatureDataset->GetSpatialReference();
	if (!SrcSpaRef.IsOk())
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Input spatial reference is not defined!"), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }
	wxGISSpatialReference DstSpaRef = wxGISSpatialReference(new OGRSpatialReference(SRS_WKT_WGS84));
	OGRFeatureDefn *pDef = pInputFeatureDataset->GetDefinition();
    if(NULL == pDef)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Error read dataset definition"), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }

    //check multi geometry
	OGRwkbGeometryType eGeomType = eFilterGeomType;
	if(eFilterGeomType == wkbUnknown)
		eGeomType = pInputFeatureDataset->GetGeometryType();

    bool bIsMultigeom = wkbFlatten(eGeomType) == wkbUnknown || wkbFlatten(eGeomType) == wkbGeometryCollection;
	if(bIsMultigeom)
	{
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Geometry collection is not supported!"), wxNOT_FOUND, enumGISMessageError);
        }
        return false;		
	}
	
	OGRFeatureDefn *pNewDef = pDef->Clone();
	if (wkbFlatten(eGeomType) > 1 && wkbFlatten(eGeomType) < 4)
	{
		eGeomType = (OGRwkbGeometryType)(eGeomType + 3);
		pNewDef->SetGeomType(eGeomType);//set multi
		if (pTrackCancel)
		{
			pTrackCancel->PutMessage(wxString::Format(_("Force geometry field to %s"), OGRGeometryTypeToName(eGeomType)), wxNOT_FOUND, enumGISMessageInformation);
		}
	}
	
	//field map
	wxVector<ST_FIELD_MAP> staFieldMap;
	int nCount = 0;
	for (size_t i = 0; i < pNewDef->GetFieldCount(); ++i)
	{
		OGRFieldDefn* pFieldDefn = pNewDef->GetFieldDefn(i);
		if (NULL != pFieldDefn)
		{
			wxString sFieldName = wxString(pFieldDefn->GetNameRef(), wxCSConv(pInputFeatureDataset->GetEncoding()));
			wxString sOldFieldName = sFieldName;
			if (IsFieldNameForbidden(sFieldName))
			{
				sFieldName = Transliterate(sFieldName); 
				wxString sAppend = wxString::Format(wxT("%.2d"), nCount + 1);
				if(sFieldName.Len() > 8)
					sFieldName = sFieldName.Left(8);
					
				sFieldName.Append(sAppend);
				pFieldDefn->SetName( sFieldName.ToUTF8() );
				if (pTrackCancel)
				{
					pTrackCancel->PutMessage(wxString::Format(_("Rename field '%s' to '%s'"), sOldFieldName.c_str(), sFieldName.c_str()), wxNOT_FOUND, enumGISMessageWarning);
				}
				

			}
			OGRFieldType eType = pFieldDefn->GetType();
			ST_FIELD_MAP record = { nCount, i, eType };
			staFieldMap.push_back(record);
			nCount++;		
		}
	}
	
	wxGxFeatureDatasetFilter SHPFilter(enumVecESRIShapefile);
	char** papszLayerOptions = NULL;
	if(pInputFeatureDataset->GetEncoding() == wxFONTENCODING_CP1251)
	{
		papszLayerOptions = CSLAddNameValue(papszLayerOptions, "ENCODING", "CP1251");
	}
	else
	{
		papszLayerOptions = CSLAddNameValue(papszLayerOptions, "ENCODING", "UTF-8");
	}
	
	CPLString osTmpPath = CPLGenerateTempFilename( "ngw" );
	
	if (!ExportFormatEx(pInputFeatureDataset, CPLGetPath(osTmpPath), wxString::FromUTF8(CPLGetBasename(osTmpPath)), &SHPFilter, wxGISNullSpatialFilter, pNewDef, staFieldMap, DstSpaRef, NULL, papszLayerOptions, true, eGeomType, true, true, pTrackCancel))
	{
		return false;
	}
	
	//2. create temp zip from temp shp, shx, dbf, prj
	
	CPLString szZipFileName = CPLResetExtension(CPLGenerateTempFilename("ngw"), "zip");
	CPLErrorReset();
	void* hZIP = CPLCreateZip(szZipFileName, NULL);
	if (!hZIP)
	{
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Zip file '%s' create failed!"), szZipFileName.c_str()), wxNOT_FOUND, enumGISMessageError);
        }
        return false;	
	}
	
	wxString sCharset(wxT("cp-866"));
	size_t nBufferSize = 1024 * 1024;
	GByte *pabyBuffer = (GByte *)CPLMalloc(nBufferSize);	
	IProgressor* pProgress = NULL;
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(_("Compress file"), wxNOT_FOUND, enumGISMessageInformation);
		pProgress = pTrackCancel->GetProgressor();
	}
	
	if(pProgress)
		pProgress->SetRange(4);
	AddFileToZip(CPLResetExtension(osTmpPath, "shp"), hZIP, &pabyBuffer, nBufferSize, "", sCharset);
	if(pProgress)
		pProgress->SetValue(1);
	AddFileToZip(CPLResetExtension(osTmpPath, "shx"), hZIP, &pabyBuffer, nBufferSize, "", sCharset);
	if(pProgress)
		pProgress->SetValue(2);
	AddFileToZip(CPLResetExtension(osTmpPath, "dbf"), hZIP, &pabyBuffer, nBufferSize, "", sCharset);
	if(pProgress)
		pProgress->SetValue(3);
	AddFileToZip(CPLResetExtension(osTmpPath, "prj"), hZIP, &pabyBuffer, nBufferSize, "", sCharset);
	if(pProgress)
		pProgress->SetValue(4);
	
	CPLCloseZip(hZIP);
	CPLFree(pabyBuffer);
	
	DeleteFile(CPLResetExtension(osTmpPath, "shp"), pTrackCancel);
	DeleteFile(CPLResetExtension(osTmpPath, "shx"), pTrackCancel);
	DeleteFile(CPLResetExtension(osTmpPath, "dbf"), pTrackCancel);
	DeleteFile(CPLResetExtension(osTmpPath, "prj"), pTrackCancel);
	
	//3. upload with progress
	
	if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Upload file"), wxNOT_FOUND, enumGISMessageTitle);
    }
	
	wxString sURL = m_pService->GetURL() + wxString(wxT("/file_upload/upload"));
    PERFORMRESULT res = curl.UploadFile(sURL, wxString::FromUTF8(szZipFileName), pTrackCancel);
	DeleteFile(szZipFileName, pTrackCancel);
	bool bResult = res.IsValid && res.nHTTPCode < 400;
	
	if(bResult)
	{
		//  "{"upload_meta": [{"id": "0eddf759-86d3-4fe0-b0f1-869fe783d2ed", "name": "ngw1_1.zip", "mime_type": "application/octet-stream", "size": 2299}]}"
  
		wxJSONReader reader;
		wxJSONValue  JSONRoot;
		int numErrors = reader.Parse(res.sBody, &JSONRoot);
		if (numErrors > 0)  {    
			if (pTrackCancel)
			{
				pTrackCancel->PutMessage(_("Unexpected error"), wxNOT_FOUND, enumGISMessageError);
			}
			return false;
		}
		
		//{"resource":{"cls":"vector_layer","parent":{"id":0},"display_name":"ggg www","keyname":null,"description":null},"vector_layer":{"srs":{"id":3857},"source":{"id":"2f906bf9-0947-45aa-b271-c711fef1d2fd","name":"ngw1_1.zip","mime_type":"application/zip","size":2299,"encoding":"utf-8"}}}
		
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Create NGW layer"), wxNOT_FOUND, enumGISMessageTitle);
        }

		wxJSONValue val;
		val["resource"]["cls"] = wxString(wxT("vector_layer"));
		val["resource"]["parent"]["id"] = m_nRemoteId;
		val["resource"]["display_name"] = sName;
		val["vector_layer"]["srs"]["id"] = 3857;
		val["vector_layer"]["source"]["id"] = JSONRoot["upload_meta"][0]["id"];
		val["vector_layer"]["source"]["name"] = JSONRoot["upload_meta"][0]["name"];
		val["vector_layer"]["source"]["mime_type"] = wxString(wxT("application/zip"));
		val["vector_layer"]["source"]["size"] = JSONRoot["upload_meta"][0]["size"];
		if(pInputFeatureDataset->GetEncoding() == wxFONTENCODING_CP1251)
			val["vector_layer"]["source"]["encoding"] = wxString(wxT("windows-1251"));
		else
			val["vector_layer"]["source"]["encoding"] = wxString(wxT("utf-8"));
		
		wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
		wxString sPayload;
		writer.Write(val, sPayload);
		
		sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
        res = curl.Post(sURL, sPayload, pTrackCancel);
		bResult = res.IsValid && res.nHTTPCode < 400;
		
		if(bResult)
		{
            numErrors = reader.Parse(res.sBody, &JSONRoot);
			if (numErrors > 0)  {    
				if (pTrackCancel)
				{
					pTrackCancel->PutMessage(_("Unexpected error"), wxNOT_FOUND, enumGISMessageError);
				}
				return false;
			}
		
            //create default style
			int nRasterLayerId = JSONRoot["id"].AsInt();
			bResult = wxGxNGWLayer::CreateDefaultStyle(m_pService, nRasterLayerId, sName, enumNGWResourceTypeVectorLayerStyle, wkbFlatten(pInputFeatureDataset->GetGeometryType()), pTrackCancel);
			OnGetUpdates();
			
			if(bResult)
				return true;	
		}		
	}
		
	wxString sErrCode = wxString::Format(_("Error code %ld"), res.nHTTPCode);
	wxString sErr;		
	wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if(numErrors > 0 || !JSONRoot.HasMember("message"))
	{
		sErr = wxString (_("Unexpected error"));
	}	
	else
	{
		sErr = JSONRoot[wxT("message")].AsString();
	}
	
	wxString sFullError = sErr + wxT(" (") + sErrCode + wxT(")");
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(sFullError, wxNOT_FOUND, enumGISMessageError);
	}
	
	return false;		
#else
	if (pTrackCancel)
	{
		pTrackCancel->PutMessage(_("Geoprocessing requered to get this functionality."), wxNOT_FOUND, enumGISMessageError);
	}

	return false;
#endif //# wxGIS_HAVE_GEOPROCESSING	
}

bool wxGxNGWResourceGroupUI::CreateRasterLayer(const wxString &sName, wxGISDataset * const pInputDataset, unsigned char R, unsigned char G, unsigned char B, unsigned char A, bool bAutoCrop, ITrackCancel* const pTrackCancel)
{
#ifdef wxGIS_HAVE_GEOPROCESSING	
    wxGISRasterDataset* pInputRasterDataset = dynamic_cast<wxGISRasterDataset*>(pInputDataset);
    if (NULL == pInputRasterDataset)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Dataset type is not supported. Raster expected."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }

    wxGISCurl curl = m_pService->GetCurl();
    if (!curl.IsOk())
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("cURL initialize failed."), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }

    //1. create temp tif file
    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Prepare GeoTiff file"), wxNOT_FOUND, enumGISMessageTitle);
    }

    wxGxRasterDatasetFilter TIFFilter(enumRasterTiff);
    char** papszOptions = NULL;
    papszOptions = CSLAddNameValue(papszOptions, "COMPRESS", "DEFLATE");
    papszOptions = CSLAddNameValue(papszOptions, "PREDICTOR", "2");
    papszOptions = CSLAddNameValue(papszOptions, "ZLEVEL", "9");

    wxArrayInt anBands;
    anBands.Add(R);
    anBands.Add(G);
    anBands.Add(B);
    wxGISEnumForceBandColorInterpretation eForceBandColorTo;
    if (bAutoCrop || A < 1)
    {
        eForceBandColorTo = enumGISForceBandsToRGB;
    }
    else
    {
        eForceBandColorTo = enumGISForceBandsToRGBA;
        anBands.Add(A);
    }

    CPLString szFileName = CPLGenerateTempFilename("ngw");
    if (!ExportFormatEx(pInputRasterDataset, CPLGetPath(szFileName), wxString::FromUTF8(CPLGetBasename(szFileName)), &TIFFilter, papszOptions, OGREnvelope(), GDT_Byte, anBands, eForceBandColorTo, true, true, pTrackCancel))
    {
        return false;
    }
	
    //2. auto crop if needed
    if (bAutoCrop)
    {
        // nearblack.cpp
    }
   
    CPLString szFilePath = CPLResetExtension(szFileName, TIFFilter.GetExt().ToUTF8());

    //3. upload with progress

    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Upload file"), wxNOT_FOUND, enumGISMessageTitle);
    }

    wxString sURL = m_pService->GetURL() + wxString(wxT("/file_upload/upload"));
    PERFORMRESULT res = curl.UploadFile(sURL, wxString::FromUTF8(szFilePath), pTrackCancel);
    DeleteFile(szFilePath, pTrackCancel);
    bool bResult = res.IsValid && res.nHTTPCode < 400;

    if (bResult)
    {
        wxJSONReader reader;
        wxJSONValue  JSONRoot;
        int numErrors = reader.Parse(res.sBody, &JSONRoot);
        if (numErrors > 0)  {
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(_("Unexpected error"), wxNOT_FOUND, enumGISMessageError);
            }
            return false;
        }


        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Create NGW layer"), wxNOT_FOUND, enumGISMessageTitle);
        }
        wxJSONValue val;
        val["resource"]["cls"] = wxString(wxT("raster_layer"));
        val["resource"]["parent"]["id"] = m_nRemoteId;
        val["resource"]["display_name"] = sName;
        val["raster_layer"]["srs"]["id"] = 3857;
        val["raster_layer"]["source"]["id"] = JSONRoot["upload_meta"][0]["id"];
        val["raster_layer"]["source"]["name"] = JSONRoot["upload_meta"][0]["name"];
        val["raster_layer"]["source"]["mime_type"] = JSONRoot["upload_meta"][0]["mime_type"];
        val["raster_layer"]["source"]["size"] = JSONRoot["upload_meta"][0]["size"];

        wxJSONWriter writer(wxJSONWRITER_NO_INDENTATION | wxJSONWRITER_NO_LINEFEEDS);
        wxString sPayload;
        writer.Write(val, sPayload);

        sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d/child/"), m_nRemoteId);
        res = curl.Post(sURL, sPayload, pTrackCancel);
        bResult = res.IsValid && res.nHTTPCode < 400;

        if (bResult)
        {
            numErrors = reader.Parse(res.sBody, &JSONRoot);
            if (numErrors > 0)  {
                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(_("Unexpected error"), wxNOT_FOUND, enumGISMessageError);
                }
                return false;
            }

            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(_("Create default layer style"), wxNOT_FOUND, enumGISMessageTitle);
            }

            //create default style
            int nRasterLayerId = JSONRoot["id"].AsInt();
            bResult = wxGxNGWLayer::CreateDefaultStyle(m_pService, nRasterLayerId, sName, enumNGWResourceTypeRasterLayerStyle, 0, pTrackCancel);
            OnGetUpdates();

            if (bResult)
                return true;
        }
    }

    wxString sErrCode = wxString::Format(_("Error code %ld"), res.nHTTPCode);
    wxString sErr;
    wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if (numErrors > 0 || !JSONRoot.HasMember("message"))
    {
        sErr = wxString(_("Unexpected error"));
    }
    else
    {
        sErr = JSONRoot["message"].AsString();
    }

    wxString sFullError = sErr + wxT(" (") + sErrCode + wxT(")");
    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(sFullError, wxNOT_FOUND, enumGISMessageError);
    }

    return false;
#else
    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Geoprocessing requered to get this functionality."), wxNOT_FOUND, enumGISMessageError);
    }

    return false;
#endif //# wxGIS_HAVE_GEOPROCESSING			
}

bool wxGxNGWResourceGroupUI::CanImport()
{
	return true;
}

bool wxGxNGWResourceGroupUI::Import(wxWindow* pWnd)
{
	//open choose dialog
	wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select input objects"));
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		wxString sPath = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/ngw_import/path"), wxEmptyString);
		if(!sPath.IsEmpty())
			dlg.SetStartingLocation(sPath);
	}
	dlg.SetAllowMultiSelect(true);
	dlg.SetAllFilters(false);
	dlg.SetOwnsFilter(true);
	dlg.SetOverwritePrompt(false);
	dlg.AddFilter(new wxGxDatasetFilter(enumGISAny), true);
	
	if(dlg.ShowModalOpen() == wxID_OK)
	{
		const wxGxObjectList lObj = dlg.GetChildren();
		wxVector<IGxDataset*> paDatasets;
		for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
		{
			wxGxObject* pGxObject = *it;
			if (NULL != pGxObject)
			{
				if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
				{						
					wxGxDatasetContainer* pCont = wxDynamicCast(pGxObject, wxGxDatasetContainer);
					if (!pCont->HasChildren(true))
						continue;
					const wxGxObjectList lObj = pCont->GetChildren();
					for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
					{
						IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(*it);
						if (NULL != pGxDSet)
						{
							paDatasets.push_back(pGxDSet);
						}
					}
				}
				else if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
				{
					paDatasets.push_back(dynamic_cast<IGxDataset*>(pGxObject));
				}
			}
		}		
		
		
		//create dialog for vector and raster config
		wxGISDatasetImportDlg dlg(this, paDatasets, pWnd);
		if(dlg.ShowModal() == wxID_OK)
		{
			size_t nCount = dlg.GetDatasetCount();
			wxGISProgressDlg ProgressDlg(_("Import selected items"), _("Begin operation..."), nCount, pWnd);
			ProgressDlg.ShowProgress(true);
			for ( size_t i = 0; i < nCount; ++i ) 
			{    
				ProgressDlg.SetValue(i);
				if(!ProgressDlg.Continue())
				{
					return false;
				}
				
				wxGISDatasetImportDlg::DATASETDESCR descr = dlg.GetDataset(i);
				if(descr.pDataset != NULL)
				{
					if(descr.pDataset->GetType() == enumGISFeatureDataset)
					{
						CreateVectorLayer(descr.sName, descr.pDataset, descr.eFilterGeometryType, &ProgressDlg);
					}
					else if(descr.pDataset->GetType() == enumGISRasterDataset)
					{
						CreateRasterLayer(descr.sName, descr.pDataset, descr.nRGBABands.R, descr.nRGBABands.G, descr.nRGBABands.B, descr.nRGBABands.A, descr.bToMultigeomOrAutoCrop, &ProgressDlg);
					}
				}
			}
			
			ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());		
			OnGetUpdates();
			return true;
		}		
	}
	return false;
}

//--------------------------------------------------------------
//class wxGxNGWLayerUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWLayerUI, wxGxNGWLayer)

wxGxNGWLayerUI::wxGxNGWLayerUI(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWLayer(pService, eType, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWLayerUI::~wxGxNGWLayerUI(void)
{
}

wxIcon wxGxNGWLayerUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWLayerUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

wxArrayString wxGxNGWLayerUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISVectorPropertyPage");	
	out.Add("wxGISSpatialReferencePropertyPage");	
	out.Add("wxGISNGWResourcePropertyPage");	
	out.Add("wxGISNGWMetaPropertyPage");	
	return out;
}

bool wxGxNGWLayerUI::HasPropertyPages(void) const
{
	return true;
}

//--------------------------------------------------------------
//class wxGxNGWRasterUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWRasterUI, wxGxNGWRaster)

wxGxNGWRasterUI::wxGxNGWRasterUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWRaster(pService, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWRasterUI::~wxGxNGWRasterUI(void)
{
}

wxIcon wxGxNGWRasterUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWRasterUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

wxArrayString wxGxNGWRasterUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISRasterPropertyPage");	
	out.Add("wxGISSpatialReferencePropertyPage");
	out.Add("wxGISNGWResourcePropertyPage");	
	out.Add("wxGISNGWMetaPropertyPage");	
	return out;
}

bool wxGxNGWRasterUI::HasPropertyPages(void) const
{
	return true;
}

//--------------------------------------------------------------
//class wxGxNGWPostGISConnectionUI
//--------------------------------------------------------------
#ifdef wxGIS_USE_POSTGRES
IMPLEMENT_CLASS(wxGxNGWPostGISConnectionUI, wxGxRemoteConnectionUI)

wxGxNGWPostGISConnectionUI::wxGxNGWPostGISConnectionUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIconConn, const wxIcon &SmallIconConn, const wxIcon &LargeIconDisconn, const wxIcon &SmallIconDisconn) : wxGxRemoteConnectionUI(oParent, soName, soPath, LargeIconConn, SmallIconConn, LargeIconDisconn, SmallIconDisconn), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypePostgisConnection;
    m_pService = pService;
    m_sName = m_sDisplayName;	
	wxString sURL = m_pService->GetURL() + wxString::Format(wxT("/resource/%d"), m_nRemoteId);
	m_sPath = CPLString(sURL.ToUTF8());
	
	wxJSONValue JSONConn = Data[wxT("postgis_connection")];
	m_sUser =  JSONConn[wxT("username")].AsString();
	m_sPass =  JSONConn[wxT("password")].AsString();
	m_sDatabase =  JSONConn[wxT("database")].AsString();
	m_sHost =  JSONConn[wxT("hostname")].AsString();
}

wxGxNGWPostGISConnectionUI::~wxGxNGWPostGISConnectionUI(void)
{
}

wxString wxGxNGWPostGISConnectionUI::GetCategory(void) const
{ 
	return wxString(_("NGW PostGIS Connection")); 
}

wxGISDataset* const wxGxNGWPostGISConnectionUI::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sUser, m_sPass, wxT("5432"), m_sHost, m_sDatabase);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

int wxGxNGWPostGISConnectionUI::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}


bool wxGxNGWPostGISConnectionUI::CanDelete(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanDelete = m_oPermissions["resource"]["delete"].AsBool();
		return bCanDelete && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnectionUI::CanRename(void)
{
    //check permissions
	if(m_oPermissions.IsValid())
	{
		bool bCanRename = m_oPermissions["resource"]["update"].AsBool();
		return bCanRename && m_pService != NULL;
	}
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnectionUI::Delete(void)
{
    if( DeleteResource() )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnectionUI::Rename(const wxString &sNewName)
{
    if( RenameResource(sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnectionUI::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWPostGISConnectionUI::CanCopy(const CPLString &szDestPath)
{
	return CanCopyResource(szDestPath);
}

bool wxGxNGWPostGISConnectionUI::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWPostGISConnectionUI::CanMove(const CPLString &szDestPath)
{
	if(!CanMoveResource(szDestPath))
		return CanCopy(szDestPath) && CanDelete();
	return true;
}

wxArrayString wxGxNGWPostGISConnectionUI::GetPropertyPages() const
{
	wxArrayString out;	
	out.Add("wxGISNGWResourcePropertyPage");	
	out.Add("wxGISNGWMetaPropertyPage");		
	return out;
}

bool wxGxNGWPostGISConnectionUI::HasPropertyPages(void) const
{
	return true;
}
#endif //wxGIS_USE_POSTGRES
//--------------------------------------------------------------
//class wxGxNGWFileSetUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWFileSetUI, wxGxNGWFileSet)

wxGxNGWFileSetUI::wxGxNGWFileSetUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWFileSet(pService, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWFileSetUI::~wxGxNGWFileSetUI(void)
{
}

wxIcon wxGxNGWFileSetUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWFileSetUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

wxArrayString wxGxNGWFileSetUI::GetPropertyPages() const
{
	wxArrayString out;	
	out.Add("wxGISNGWResourcePropertyPage");	
	out.Add("wxGISNGWMetaPropertyPage");		
	return out;
}

bool wxGxNGWFileSetUI::HasPropertyPages(void) const
{
	return true;
}


#endif // wxGIS_USE_CURL
