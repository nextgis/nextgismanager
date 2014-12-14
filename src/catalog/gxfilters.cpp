/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/catalog/gxngwconn.h"
#include "wxgis/catalog/gxremoteconn.h"
#include "wxgis/datasource/sysop.h"

/*#include "wxgis/catalog/gxspatreffolder.h"
*/

//#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
//WX_DEFINE_USER_EXPORTED_ARRAY(wxGxObjectFiltersArray);

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------
IMPLEMENT_CLASS(wxGxObjectFilter, wxObject)

wxGxObjectFilter::wxGxObjectFilter(void)
{
}

wxGxObjectFilter::~wxGxObjectFilter(void)
{
}

bool wxGxObjectFilter::CanChooseObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	return true;
}

bool wxGxObjectFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	return true;
}

bool wxGxObjectFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	return true;
}

wxGISEnumSaveObjectResults wxGxObjectFilter::CanSaveObject( wxGxObject* const pLocation, const wxString &sName )
{
	wxGxObjectContainer* pContainer = wxDynamicCast(pLocation, wxGxObjectContainer);
	if(!pContainer)
		return enumGISSaveObjectDeny;
	if(pContainer->CanCreate(GetType(), GetSubType()))
	{
        wxString sDir = pLocation->GetFullName();
        if (sDir.EndsWith(wxFileName::GetPathSeparator()))
        {
            sDir += sName;
        }
        else
        {
            sDir += wxFileName::GetPathSeparator() + sName;
        }
        if (pContainer->FindGxObject(sDir) == NULL)
			return enumGISSaveObjectAccept;
		else
			return enumGISSaveObjectExists;
	}
	return enumGISSaveObjectDeny;
}

wxString wxGxObjectFilter::GetName(void) const
{
	return wxString(_("Any items (*.*)"));
}

wxString wxGxObjectFilter::GetExt(void) const
{
	return wxEmptyString;
}

wxString wxGxObjectFilter::GetDriver(void) const
{
	return wxEmptyString;
}

int wxGxObjectFilter::GetSubType(void) const
{
    return 0;
}

wxGISEnumDatasetType wxGxObjectFilter::GetType(void) const
{
    return enumGISAny;
}

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPrjFileFilter, wxGxObjectFilter)

wxGxPrjFileFilter::wxGxPrjFileFilter(void)
{
}

wxGxPrjFileFilter::~wxGxPrjFileFilter(void)
{
}

bool wxGxPrjFileFilter::CanChooseObject( wxGxObject* const pObject )
{
	wxGxPrjFile* pGxPrjFile = wxDynamicCast(pObject, wxGxPrjFile);
	if(pGxPrjFile)
		return true;
	else
		return false;
}

bool wxGxPrjFileFilter::CanDisplayObject( wxGxObject* const pObject )
{
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
		return true;
	return CanChooseObject(pObject);
}

wxString wxGxPrjFileFilter::GetName(void) const
{
	return wxString(_("Coordinate Systems (*.prj, *.qpj, *.spr)"));
}

wxString wxGxPrjFileFilter::GetExt(void) const
{
	return wxString(wxT("spr"));
}

//------------------------------------------------------------
// wxGxRemoteDBSchemaFilter
//------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteDBSchemaFilter, wxGxObjectFilter)

wxGxRemoteDBSchemaFilter::wxGxRemoteDBSchemaFilter(void)
{
}

wxGxRemoteDBSchemaFilter::~wxGxRemoteDBSchemaFilter(void)
{
}

bool wxGxRemoteDBSchemaFilter::CanChooseObject(wxGxObject* const pObject)
{
#ifdef wxGIS_USE_POSTGRES
    wxGxRemoteDBSchema* pContainer2 = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
    if (pContainer2)
        return true;
#endif // wxGIS_USE_POSTGRES
    return false;
}

bool wxGxRemoteDBSchemaFilter::CanDisplayObject(wxGxObject* const pObject)
{
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;
#ifdef wxGIS_USE_POSTGRES
    wxGxRemoteConnection* pContainer1 = dynamic_cast<wxGxRemoteConnection*>(pObject);
    if (pContainer1)
        return true;
    wxGxRemoteDBSchema* pContainer2 = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
    if (pContainer2)
        return true;
#endif // wxGIS_USE_POSTGRES
    return false;
}

bool wxGxRemoteDBSchemaFilter::CanStoreToObject(wxGxObject* const pObject)
{
#ifdef wxGIS_USE_POSTGRES
    wxGxRemoteConnection* pContainer1 = dynamic_cast<wxGxRemoteConnection*>(pObject);
    if (pContainer1)
        return true;
#endif // wxGIS_USE_POSTGRES
    return false;
}

wxString wxGxRemoteDBSchemaFilter::GetName(void) const
{
    return wxString(_("Database schema"));
}

//------------------------------------------------------------
// wxGxDatasetFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxDatasetFilter, wxGxObjectFilter)

wxGxDatasetFilter::wxGxDatasetFilter(wxGISEnumDatasetType nType, int nSubType)
{
    m_nType = nType;
	m_nSubType = nSubType;
}


wxGxDatasetFilter::~wxGxDatasetFilter(void)
{
}

bool wxGxDatasetFilter::CanChooseObject( wxGxObject* const pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
	if(m_nType == enumGISAny)
		return true;
    if(pGxDataset->GetType() != m_nType)
		return false;
	if(m_nSubType != wxNOT_FOUND)
		if(pGxDataset->GetSubType() != GetSubType())
			return false;
    return true;
}

bool wxGxDatasetFilter::CanDisplayObject( wxGxObject* const pObject )
{
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
	if(m_nType == enumGISAny)
		return true;
    if(pGxDataset->GetType() != m_nType)
		return false;
	if(m_nSubType != wxNOT_FOUND)
		if(pGxDataset->GetSubType() != GetSubType())
			return false;
    return true;
}

bool wxGxDatasetFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (IsFileDataset(m_nType, GetSubType()))
    {
        wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
        if (pContainer)
            return true;
    }
    else
    {
#ifdef wxGIS_USE_POSTGRES
        wxGxRemoteDBSchema* pContainer = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
        if (pContainer)
            return true;
#endif // wxGIS_USE_POSTGRES
    }
    return false;
}

wxString wxGxDatasetFilter::GetName(void) const
{
    switch(m_nType)
    {
	case enumGISAny:
		return wxString(_("Any raster, vector, table, container"));
    case enumGISRasterDataset:
        return wxString(_("Raster (*.img, *.tif, etc.)"));
    case enumGISFeatureDataset:
        return wxString(_("Vector (*.shp, *.tab, etc.)"));
    case enumGISTable:
        return wxString(_("Table (*.dbf, *.tab, *.csv, etc.)"));
    }
    return wxEmptyString;
}

//------------------------------------------------------------
// wxGxFeatureFileFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFeatureDatasetFilter, wxGxObjectFilter)

wxGxFeatureDatasetFilter::wxGxFeatureDatasetFilter(wxGISEnumVectorDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxFeatureDatasetFilter::~wxGxFeatureDatasetFilter(void)
{
}

bool wxGxFeatureDatasetFilter::CanChooseObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(GetSubType() == enumVecUnknown)
        return true;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxFeatureDatasetFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;
	wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(GetSubType() == enumVecUnknown)
        return true;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxFeatureDatasetFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (IsFileDataset(enumGISFeatureDataset, GetSubType()))
    {
        wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
        if (pContainer)
            return true;
    }
    else
    {
#ifdef wxGIS_USE_POSTGRES
        wxGxRemoteDBSchema* pContainer = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
        if (pContainer)
            return true;
#endif // wxGIS_USE_POSTGRES
    }
    return false;
}

wxString wxGxFeatureDatasetFilter::GetName(void) const
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
        return wxString(_("ESRI Shapefile (*.shp)"));
    case enumVecMapinfoTab:
        return wxString(_("MapInfo File (*.tab)"));
    case enumVecMapinfoMif:
        return wxString(_("MapInfo File (*.mid/*.mif)"));
    case enumVecKML:
 	    return wxString(_("KML file (*.kml)"));
    case enumVecKMZ:
 	    return wxString(_("KML file (*.kmz)"));
    case enumVecDXF:
	    return wxString(_("AutoCAD DXF file (*.dxf)"));
	case enumVecPostGIS:
	    return wxString(_("PostGIS Feature class"));
	case enumVecGML:
	    return wxString(_("GML file (*.gml)"));
	case enumVecGeoJSON:
	    return wxString(_("GeoJSON file (*.geojson)"));
	case enumVecWFS:
	    return wxString(_("WFS"));
	case enumVecMem:
	    return wxString(_("Memory"));
    case enumVecSXF:
	    return wxString(_("Panorama SXF (*.sxf)"));
    case enumVecS57:
	    return wxString(_("s57 format (*.000)"));
    case enumVecCSV:
 	    return wxString(_("Comma Separated Values (*.csv)"));
    case enumVecFileDBLayer:
	    return wxString(_("GeoDatabase Feature class"));
    default:
	    return wxString(_("Any feature classes (*.*)"));
    }
}

wxString wxGxFeatureDatasetFilter::GetExt(void) const
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
	    return wxString(wxT("shp"));
    case enumVecMapinfoTab:
	    return wxString(wxT("tab"));
    case enumVecMapinfoMif:
        return wxString(wxT("mif"));
    case enumVecKML:
        return wxString(wxT("kml"));
    case enumVecKMZ:
        return wxString(wxT("kmz"));
    case enumVecDXF:
        return wxString(wxT("dxf"));
	case enumVecWFS:
	case enumVecMem:
	case enumVecPostGIS:
    case enumVecFileDBLayer:
	    return wxEmptyString;
	case enumVecGML:
	    return wxString(wxT("gml"));
	case enumVecGeoJSON:
	    return wxString(wxT("geojson"));
    case enumVecSXF:
	    return wxString(wxT("sxf"));
    case enumVecS57:
	    return wxString(wxT("000"));
    case enumVecCSV:
	    return wxString(wxT("csv"));
    default:
        return wxEmptyString;
    }
}

wxString wxGxFeatureDatasetFilter::GetDriver(void) const
{
    return GetDriverByType(GetType(), m_nSubType);
}

int wxGxFeatureDatasetFilter::GetSubType(void) const
{
    return m_nSubType;
}

//------------------------------------------------------------
// wxGxFolderFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFolderFilter, wxGxObjectFilter)

wxGxFolderFilter::wxGxFolderFilter(void)
{
}

wxGxFolderFilter::~wxGxFolderFilter(void)
{
}

bool wxGxFolderFilter::CanChooseObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    //if(pObject->GetCategory() == wxString(_("Folder")))
    //    return true;
    //if(pObject->GetCategory() == wxString(_("Folder connection")))
    //    return true;
    if(pObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
		return true;
    return false;
}

bool wxGxFolderFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;    
  //  if(pObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
		//return true;
  //  if(pObject->GetCategory() == wxString(wxT("Root")))
  //      return true;
  //  if(pObject->GetCategory() == wxString(_("Folder")))
  //      return true;
  //  if(pObject->GetCategory() == wxString(_("Folder connection")))
  //      return true;

    return CanChooseObject(pObject);
}

bool wxGxFolderFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
    if (pContainer)
        return true;
    return false;

}

wxString wxGxFolderFilter::GetName(void) const
{
	return wxString(_("Folder"));
}

//------------------------------------------------------------
// wxGxRasterDatasetFilter
//------------------------------------------------------------

wxGxRasterDatasetFilter::wxGxRasterDatasetFilter(wxGISEnumRasterDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxRasterDatasetFilter::~wxGxRasterDatasetFilter(void)
{
}

bool wxGxRasterDatasetFilter::CanChooseObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
    if (!pGxDataset)
        return false;
    if (pGxDataset->GetType() != GetType())
        return false;
    if (GetSubType() == enumRasterUnknown)
        return true;
    if (pGxDataset->GetSubType() != GetSubType())
        return false;
    return true;
}

bool wxGxRasterDatasetFilter::CanDisplayObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;    
    return CanChooseObject(pObject);
}

bool wxGxRasterDatasetFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (IsFileDataset(enumGISRasterDataset, GetSubType()))
    {
        wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
        if (pContainer)
            return true;
    }
    else
    {
#ifdef wxGIS_USE_POSTGRES
        wxGxRemoteDBSchema* pContainer = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
        if (pContainer)
            return true;
#endif // wxGIS_USE_POSTGRES
    }
    return false;
}

wxString wxGxRasterDatasetFilter::GetName(void) const
{
    switch (m_nSubType)
    {
    case enumRasterBmp:
        return wxString(_("Windows Device Independent Bitmap (*.bmp)"));
    case enumRasterTiff:
        return wxString(_("GeoTIFF (*.tif, *.tiff)"));
    case enumRasterJpeg:
        return wxString(_("JPEG image (*.jpeg, *.jfif, *.jpg, *.jpe)"));
    case enumRasterImg:
        return wxString(_("Erdas IMAGINE image (*.img)"));
    case enumRasterPng:
        return wxString(_("Portable Network Graphics (*.png)"));
    case enumRasterGif:
        return wxString(_("Graphics Interchange Format (*.gif)"));
    case enumRasterSAGA:
        return wxString(_("SAGA GIS Binary Grid (*.sdat)"));
    case enumRasterTil:
        return wxString(_("EarthWatch raster (*.til)"));
    case enumRasterVRT:
        return wxString(_("Virtual raster (*.vrt)"));
    case enumRasterWMS:
        return wxString(_("Web map service"));
    case enumRasterWMSTMS:
        return wxString(_("Tile map service"));
    case enumRasterPostGIS:
        return wxString(_("PostGIS raster"));
    case enumRasterFileDBLayer:
        return wxString(_("GDB raster"));
    default:
        return wxString(_("Any rasters (*.*)"));
    }
}

wxString wxGxRasterDatasetFilter::GetExt(void) const
{
    switch (m_nSubType)
    {
    case enumRasterBmp:
        return wxString(wxT("bmp"));
    case enumRasterTiff:
        return wxString(wxT("tif"));
    case enumRasterJpeg:
        return wxString(wxT("jpg"));
    case enumRasterImg:
        return wxString(wxT("img"));
    case enumRasterPng:
        return wxString(wxT("png"));
    case enumRasterGif:
        return wxString(wxT("gif"));
    case enumRasterSAGA:
        return wxString(wxT("sdat"));
    case enumRasterTil:
        return wxString(wxT("til"));
    case enumRasterVRT:
        return wxString(wxT("vrt"));
    case enumRasterWMS:
    case enumRasterWMSTMS:
    case enumRasterPostGIS:
    case enumRasterFileDBLayer:
        return wxEmptyString;
    default:
        return wxEmptyString;
    }
}

wxString wxGxRasterDatasetFilter::GetDriver(void) const
{
    return GetDriverByType(GetType(), m_nSubType);
}

int wxGxRasterDatasetFilter::GetSubType(void) const
{
    return m_nSubType;
}

//------------------------------------------------------------
// wxGxTextFilter
//------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTextFilter, wxGxObjectFilter)

wxGxTextFilter::wxGxTextFilter(const wxString &soName, const wxString &soExt)
{
    m_soName = soName;
    m_soExt = soExt;
    m_soExtCmp = soExt.Right(soExt.Len() - 1);//remove dot from extension
}

wxGxTextFilter::~wxGxTextFilter(void)
{
}

bool wxGxTextFilter::CanChooseObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	wxGxTextFile* poGxTextFile = wxDynamicCast(pObject, wxGxTextFile);
	if(!poGxTextFile)
		return false;
    if(wxGISEQUAL(CPLGetExtension(poGxTextFile->GetPath()), m_soExtCmp.mb_str(wxConvUTF8)))
		return true;
	return false;
}

bool wxGxTextFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;
    return CanChooseObject(pObject);
}

bool wxGxTextFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
    if (pContainer)
        return true;
    return false;

}

wxString wxGxTextFilter::GetName(void) const
{
	return m_soName;
}

wxString wxGxTextFilter::GetExt(void) const
{
	return m_soExt;
}


//------------------------------------------------------------
// wxGxTableFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTableFilter, wxGxObjectFilter)

wxGxTableFilter::wxGxTableFilter(wxGISEnumTableType nSubType)
{
    m_nSubType = nSubType;
}

wxGxTableFilter::~wxGxTableFilter(void)
{
}

bool wxGxTableFilter::CanChooseObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(GetSubType() == enumTableUnknown)
        return true;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxTableFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;
    return CanChooseObject(pObject);
}

bool wxGxTableFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (IsFileDataset(enumGISFeatureDataset, GetSubType()))
    {
        wxGxFolder* pContainer = dynamic_cast<wxGxFolder*>(pObject);
        if (pContainer)
            return true;
    }
    else
    {
#ifdef wxGIS_USE_POSTGRES
        wxGxRemoteDBSchema* pContainer = dynamic_cast<wxGxRemoteDBSchema*>(pObject);
        if (pContainer)
            return true;
#endif // wxGIS_USE_POSTGRES
    }
    return false;
}

wxString wxGxTableFilter::GetName(void) const
{
    switch(m_nSubType)
    {
    case enumTableDBF:
        return wxString(_("dBase file (*.dbf)"));
    case enumTableMapinfoTab:
        return wxString(_("MapInfo table File (*.tab)"));
    case enumTableMapinfoMif:
        return wxString(_("MapInfo table File (*.mif)"));
    case enumTableCSV:
 	    return wxString(_("Comma Separated Values (*.csv)"));
    case enumTableODS:
 	    return wxString(_("LibreOffice spreadsheet (*.ods)"));
    case enumTableXLS:
 	    return wxString(_("Microsoft Office Excel (*.xls)"));
    case enumTableXLSX:
 	    return wxString(_("Microsoft Office Excel (*.xlsx)"));
    case enumTablePostgres:
    case enumTableFileDBLayer:
 	    return wxString(_("Database table"));
    default:
	    return wxString(_("Any feature classes (*.*)"));
    }
}

wxString wxGxTableFilter::GetExt(void) const
{
    switch(m_nSubType)
    {
    case enumTableDBF:
	    return wxString(wxT("dbf"));
    case enumTableMapinfoTab:
	    return wxString(wxT("tab"));
    case enumTableMapinfoMif:
        return wxString(wxT("mif"));
    case enumTableCSV:
        return wxString(wxT("csv"));
    case enumTableODS:
        return wxString(wxT("ods"));
    case enumTableXLS:
        return wxString(wxT("xls"));
    case enumTableXLSX:
        return wxString(wxT("xlsx"));
    case enumTablePostgres:
    case enumTableFileDBLayer:
        return wxEmptyString;
    default:
        return wxEmptyString;
    }
}

wxString wxGxTableFilter::GetDriver(void) const
{
    return GetDriverByType(GetType(), m_nSubType);
}

int wxGxTableFilter::GetSubType(void) const
{
    return m_nSubType;
}


//------------------------------------------------------------
// wxGxNGWResourceGroupFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWResourceGroupFilter, wxGxObjectFilter)

wxGxNGWResourceGroupFilter::wxGxNGWResourceGroupFilter(void)
{
}

wxGxNGWResourceGroupFilter::~wxGxNGWResourceGroupFilter(void)
{
}

bool wxGxNGWResourceGroupFilter::CanChooseObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (pObject->IsKindOf(wxCLASSINFO(wxGxNGWResourceGroup)))
        return true;
    return false;
}

bool wxGxNGWResourceGroupFilter::CanDisplayObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;    
    if (pObject->IsKindOf(wxCLASSINFO(wxGxNGWService)))
        return true;
    return CanChooseObject(pObject);
}

bool wxGxNGWResourceGroupFilter::CanStoreToObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    return pObject->IsKindOf(wxCLASSINFO(wxGxNGWResourceGroup));
}

wxString wxGxNGWResourceGroupFilter::GetName(void) const
{
    return wxString(_("NGW Resource group"));
}

//------------------------------------------------------------
// wxGxSearchObjectFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxSearchObjectFilter, wxGxObjectFilter)

wxGxSearchObjectFilter::wxGxSearchObjectFilter(void)
{
}

wxGxSearchObjectFilter::~wxGxSearchObjectFilter(void)
{
}

bool wxGxSearchObjectFilter::CanChooseObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxSearchObject*>(pObject) != NULL)
        return true;
    return false;
}

bool wxGxSearchObjectFilter::CanDisplayObject(wxGxObject* const pObject)
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;    

    return CanChooseObject(pObject);
}

bool wxGxSearchObjectFilter::CanStoreToObject(wxGxObject* const pObject)
{    
    return false;
}

wxString wxGxSearchObjectFilter::GetName(void) const
{
    return wxString(_("Search source"));
}