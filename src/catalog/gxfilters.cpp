/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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

#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxfile.h"
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

wxGISEnumSaveObjectResults wxGxObjectFilter::CanSaveObject( wxGxObject* const pLocation, const wxString &sName )
{
	wxGxObjectContainer* pContainer = wxDynamicCast(pLocation, wxGxObjectContainer);
	if(!pContainer)
		return enumGISSaveObjectDeny;
	if(pContainer->CanCreate(GetType(), GetSubType()))
	{
        if(pContainer->FindGxObject(pLocation->GetFullName() + wxFileName::GetPathSeparator() + sName) == NULL)
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
/*
IMPLEMENT_CLASS(wxGxPrjFileFilter, wxGxObjectFilter)

wxGxPrjFileFilter::wxGxPrjFileFilter(void)
{
}

wxGxPrjFileFilter::~wxGxPrjFileFilter(void)
{
}

bool wxGxPrjFileFilter::CanChooseObject( wxGxObject* const pObject )
{
	wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pObject);
	if(pGxPrjFile)
		return true;
	else
		return false;
}

bool wxGxPrjFileFilter::CanDisplayObject( wxGxObject* const pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pObject);
	if(pGxPrjFile)
		return true;
	else
		return false;
}

wxString wxGxPrjFileFilter::GetName(void) const
{
	return wxString(_("Coordinate Systems (*.prj, *.spr)"));
}

wxString wxGxPrjFileFilter::GetExt(void)) const
{
	return wxString(wxT("spr"));
}
*/

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
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
	wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != m_nType)
		return false;
	if(m_nSubType != wxNOT_FOUND)
		if(pGxDataset->GetSubType() != GetSubType())
			return false;
    return true;
}

bool wxGxDatasetFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
		return true;
	wxGxDataset* pGxDataset = wxDynamicCast(pObject, wxGxDataset);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != m_nType)
		return false;
	if(m_nSubType != wxNOT_FOUND)
		if(pGxDataset->GetSubType() != GetSubType())
			return false;
    return true;
}

wxString wxGxDatasetFilter::GetName(void) const
{
    switch(m_nType)
    {
    case enumGISRasterDataset:
        return wxString(_("Raster (*.img, *.tif, etc.)"));
    case enumGISFeatureDataset:
        return wxString(_("Vector (*.shp, *.tab, etc.)"));
    case enumGISTableDataset:
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
	case emumVecPostGIS:
	    return wxString(_("PostGIS Feature class"));
	case enumVecGML:
	    return wxString(_("GML file (*.gml)"));
	case enumVecGeoJSON:
	    return wxString(_("GeoJSON file (*.geojson)"));
	case enumVecWFS:
	    return wxString(_("WFS"));
	case enumVecMem:
	    return wxString(_("Memory"));
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
	case emumVecPostGIS:
	    return wxEmptyString;
	case enumVecGML:
	    return wxString(wxT("gml"));
	case enumVecGeoJSON:
	    return wxString(wxT("geojson"));
    default:
        return wxEmptyString;
    }
}

wxString wxGxFeatureDatasetFilter::GetDriver(void) const
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
	    return wxString(wxT("ESRI Shapefile"));
    case enumVecMapinfoTab:
    case enumVecMapinfoMif:
	    return wxString(wxT("MapInfo File"));
    case enumVecKML:
    case enumVecKMZ:
	    return wxString(wxT("LIBKML"));
    case enumVecDXF:
	    return wxString(wxT("DXF"));
	case emumVecPostGIS:
	    return wxString(wxT("PostgreSQL"));
	case enumVecGML:
	    return wxString(wxT("GML"));
	case enumVecWFS:
	    return wxString(wxT("WFS"));
	case enumVecMem:
	    return wxString(wxT("Memory"));
	case enumVecGeoJSON:
	    return wxString(wxT("GeoJSON"));
    default:
        return wxEmptyString;
    }
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
   if(!pObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
		return false;
    if(pObject->GetCategory() == wxString(_("Folder")))
        return true;
    if(pObject->GetCategory() == wxString(_("Folder connection")))
        return true;
    return false;
}

bool wxGxFolderFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if(pObject->GetCategory() == wxString(wxT("Root")))
        return true;
    if(!pObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
		return false;
    if(pObject->GetCategory() == wxString(_("Folder")))
        return true;
    if(pObject->GetCategory() == wxString(_("Folder connection")))
        return true;
    return false;
}

wxString wxGxFolderFilter::GetName(void) const
{
	return wxString(_("Folder"));
}

//------------------------------------------------------------
// wxGxRasterFilter
//------------------------------------------------------------
/*
wxGxRasterFilter::wxGxRasterFilter(wxGISEnumRasterDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxRasterFilter::~wxGxRasterFilter(void)
{
}

bool wxGxRasterFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxRasterFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

wxString wxGxRasterFilter::GetName(void)
{
    switch(m_nSubType)
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
	default:
		return wxEmptyString;
    }
}

wxString wxGxRasterFilter::GetExt(void)
{
    switch(m_nSubType)
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
 	default:
		return wxEmptyString;
   }
}

wxString wxGxRasterFilter::GetDriver(void)
{
    switch(m_nSubType)
    {
    case enumRasterBmp:
        return wxString(wxT("BMP"));
    case enumRasterTiff:
	    return wxString(wxT("GTiff"));
    case enumRasterJpeg:
	    return wxString(wxT("JPEG"));
    case enumRasterImg:
	    return wxString(wxT("HFA"));
    case enumRasterPng:
	    return wxString(wxT("PNG"));
    case enumRasterGif:
	    return wxString(wxT("GIF"));
    case enumRasterSAGA:
	    return wxString(wxT("SAGA"));
    case enumRasterTil:
	    return wxString(wxT("TIL"));
	default:
		return wxEmptyString;
    }
}

int wxGxRasterFilter::GetSubType(void)
{
    return m_nSubType;
}

*/

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
	wxGxTextFile* poGxTextFile = wxDynamicCast(pObject, wxGxTextFile);
	if(!poGxTextFile)
		return false;
    if(wxGISEQUAL(CPLGetExtension(poGxTextFile->GetPath()), m_soExtCmp.mb_str(wxConvUTF8)))
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
// wxGxTableDatasetFilter
//------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTableDatasetFilter, wxGxObjectFilter)

wxGxTableDatasetFilter::wxGxTableDatasetFilter(wxGISEnumTableDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxTableDatasetFilter::~wxGxTableDatasetFilter(void)
{
}

bool wxGxTableDatasetFilter::CanChooseObject( wxGxObject* const pObject )
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

bool wxGxTableDatasetFilter::CanDisplayObject( wxGxObject* const pObject )
{
    wxCHECK_MSG(pObject, false, wxT("Input pObject pointer is NULL"));
    if (dynamic_cast<IGxObjectNoFilter*>(pObject) != NULL)
        return true;
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

wxString wxGxTableDatasetFilter::GetName(void) const
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
    case enumTablePostgres:
 	    return wxString(_("Database table"));
    default:
	    return wxString(_("Any feature classes (*.*)"));
    }
}

wxString wxGxTableDatasetFilter::GetExt(void) const
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
    case enumTablePostgres:
        return wxEmptyString;
    default:
        return wxEmptyString;
    }
}

wxString wxGxTableDatasetFilter::GetDriver(void) const
{
    switch(m_nSubType)
    {
    case enumTableDBF:
	    return wxString(wxT("ESRI Shapefile"));
    case enumTableMapinfoTab:
    case enumTableMapinfoMif:
	    return wxString(wxT("MapInfo File"));
    case enumTableCSV:
	    return wxString(wxT("CSV"));
    case enumTablePostgres:
	    return wxString(wxT("PostgreSQL"));
	//case emumVecPostGIS:
    default:
        return wxEmptyString;
    }
}

int wxGxTableDatasetFilter::GetSubType(void) const
{
    return m_nSubType;
}
