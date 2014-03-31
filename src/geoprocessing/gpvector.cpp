/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main dataset functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/catop.h"

#include "wxgisdefs.h"

#ifdef wxGIS_USE_POSTGRES
#include "wxgis/datasource/postgisdataset.h"
#endif // wxGIS_USE_POSTGRES

#include <wx/fontmap.h>
#include <wx/encconv.h>

#define MAX_FEATURES_FORINSERT 1000

bool CopyRows(wxGISTable* const pSrcDataSet, wxGISTable* const pDstDataSet, ITrackCancel* const pTrackCancel)
{
    //progress & messages
    IProgressor* pProgressor(NULL);
    if (pTrackCancel)
    {
        pProgressor = pTrackCancel->GetProgressor();
    }

    int nCounter(0);
    if (pProgressor)
    {
        pProgressor->SetRange(pSrcDataSet->GetFeatureCount(false, pTrackCancel));
    }

    wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();
    CPLErrorReset();

    //create src -> dst field mapping

    OGRFeatureDefn *pFeatureDefn = pDstDataSet->GetDefinition();

    if (NULL == pFeatureDefn)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Input dataset is corrupt"), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    //TODO: move field map to create dataset and export function as input param
    struct _st_field_map
    {
        unsigned nDstFieldNo;
        unsigned nSrsFieldNo;
        OGRFieldType eFieldType;
    };

    wxVector<_st_field_map> staFieldMap;

    OGRFeatureDefn *pSrcFeatureDefn = pSrcDataSet->GetDefinition();
    for (size_t i = 0; i < pFeatureDefn->GetFieldCount(); ++i)
    {
        OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
        if (NULL != pFieldDefn)
        {
            if (pSrcFeatureDefn->GetFieldCount() > i)
                //                int nSrcField = pSrcFeatureDefn->GetFieldIndex(pFieldDefn->GetNameRef());
                //                if (nSrcField != wxNOT_FOUND)
            {
                OGRFieldType eType = pFieldDefn->GetType();
                _st_field_map record = { i, i, eType };
                staFieldMap.push_back(record);
            }
        }
    }

#ifndef CPL_RECODE_ICONV
    wxEncodingConverter oEncConverter;
    bool bFastConv = oEncConverter.Init(pSrcDataSet->GetEncoding(), pDstDataSet->GetEncoding(), wxCONVERT_SUBSTITUTE);
    char szMaxStr[4096];
#endif

    pSrcDataSet->Reset();
    wxGISFeature Feature;
    size_t i;
    OGRwkbGeometryType eGeoFieldtype = pFeatureDefn->GetGeomType();
    while ((Feature = pSrcDataSet->Next()).IsOk())
    {
        if (pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.ToUTF8());
            CPLError(CE_Warning, CPLE_AppDefined, sFullErr);

            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }


        wxGISFeature newFeature = pDstDataSet->CreateFeature();

        for (i = 0; i < staFieldMap.size(); ++i)
        {
            switch (staFieldMap[i].eFieldType)
            {
            case OFTRealList:
                newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDoubleList(staFieldMap[i].nSrsFieldNo));
                break;
            case OFTIntegerList:
                newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsIntegerList(staFieldMap[i].nSrsFieldNo));
                break;
            case OFTStringList:
                newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsStringList(staFieldMap[i].nSrsFieldNo));
                break;
            case OFTString:
#ifdef CPL_RECODE_ICONV
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsString(staFieldMap[i].nSrsFieldNo));
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsChar(staFieldMap[i].nSrsFieldNo));
                //                    break;
#else
                if (bFastConv)
                {
                    const char* pszStr = Feature.GetFieldAsChar(staFieldMap[i].nSrsFieldNo);
                    if (oEncConverter.Convert(pszStr, szMaxStr))
                    {
                        newFeature.SetField(staFieldMap[i].nDstFieldNo, szMaxStr);
                        break;
                    }
                }
#endif //CPL_RECODE_ICONV
            case OFTDate:
            case OFTTime:
            case OFTDateTime:
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDateTime(staFieldMap[i].nSrsFieldNo));
                //                    break;
            case OFTReal:
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDouble(staFieldMap[i].nSrsFieldNo));
                //                    break;
            case OFTInteger:
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsInteger(staFieldMap[i].nSrsFieldNo));
                //                    break;
            default:
                //TODO: recode to output encoding if pDstDataSet encoding diffes from src
                newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetRawField(staFieldMap[i].nSrsFieldNo));
                //                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsString(staFieldMap[i].nSrsFieldNo));
                break;
            };
        }

        OGRErr eErr = pDstDataSet->StoreFeature(newFeature);
        if (eErr != OGRERR_NONE)
        {
            wxString sErr = wxString::Format(_("Error create feature!\nSource feature FID:%d\nOGR error: "), Feature.GetFID());
            CPLString sFullErr(sErr.mb_str(wxConvUTF8));
            sFullErr += CPLGetLastErrorMsg();
            CPLError(CE_Failure, CPLE_AppDefined, sFullErr);
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
            }
        }

        if (pProgressor)
        {
            pProgressor->SetValue(nCounter++);
        }
    }

    return true;
}

bool CopyRows(wxGISFeatureDataset* const pSrcDataSet, wxGISFeatureDataset* const pDstDataSet, ITrackCancel* const pTrackCancel)
{
    const wxGISSpatialReference oSrcSRS = pSrcDataSet->GetSpatialReference();
    const wxGISSpatialReference oDstSRS = pDstDataSet->GetSpatialReference();
    OGRCoordinateTransformation *poCT = NULL;
    
    if (oSrcSRS.IsOk() && oDstSRS.IsOk() && !oSrcSRS.IsSame(oDstSRS))
    {
        poCT = OGRCreateCoordinateTransformation(oSrcSRS, oDstSRS);
    }

    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
    }

    int nCounter(0);
    if (pProgressor)
    {
        pProgressor->SetRange(pSrcDataSet->GetFeatureCount(false, pTrackCancel));
    }

    wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();
    CPLErrorReset();

    //create src -> dst field mapping

    OGRFeatureDefn *pFeatureDefn = pDstDataSet->GetDefinition();

    if (NULL == pFeatureDefn)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Input dataset is corrupt"), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    //TODO: move field map to create dataset and export function as input param
    struct _st_field_map
    {
        unsigned nDstFieldNo;
        unsigned nSrsFieldNo;
        OGRFieldType eFieldType;
    };

    wxVector<_st_field_map> staFieldMap;

    OGRFeatureDefn *pSrcFeatureDefn = pSrcDataSet->GetDefinition();
    if (pDstDataSet->GetSubType() == enumVecDXF)
    {
        //add name, desc, descript, descritpion to output string
        for (size_t i = 0; i < pSrcFeatureDefn->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pFieldDefn = pSrcFeatureDefn->GetFieldDefn(i);
            if (NULL != pFieldDefn)
            {
                wxString sName(pFieldDefn->GetNameRef(), wxConvUTF8);
                sName.MakeLower();
                if (pFieldDefn->GetType() == OFTString)
                {
                    if (sName.StartsWith(wxT("name")) || sName.StartsWith(wxT("desc")) || sName.StartsWith(wxT("label")))
                    {
                        _st_field_map record = { wxNOT_FOUND, i, OFTString };
                        staFieldMap.push_back(record);
                    }
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < pFeatureDefn->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
            if (NULL != pFieldDefn)
            {
                if (pSrcFeatureDefn->GetFieldCount() > i)
//                int nSrcField = pSrcFeatureDefn->GetFieldIndex(pFieldDefn->GetNameRef());
//                if (nSrcField != wxNOT_FOUND)
                {
                    OGRFieldType eType = pFieldDefn->GetType();
                    _st_field_map record = { i, i, eType };
                    staFieldMap.push_back(record);
                }
            }
        }
    }

#ifndef CPL_RECODE_ICONV
    wxEncodingConverter oEncConverter;
    bool bFastConv = oEncConverter.Init(pSrcDataSet->GetEncoding(), pDstDataSet->GetEncoding(), wxCONVERT_SUBSTITUTE);
    char szMaxStr[4096];
#endif

    pSrcDataSet->Reset();
    wxGISFeature Feature;
    size_t i;
    OGRwkbGeometryType eGeoFieldtype = pFeatureDefn->GetGeomType();
    while ((Feature = pSrcDataSet->Next()).IsOk())
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.ToUTF8());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }


        wxGISFeature newFeature = pDstDataSet->CreateFeature();
        
        //set geometry
        wxGISGeometry Geom = Feature.GetGeometry();
        if (Geom.IsOk())
        {
            OGRGeometry *pNewGeom = NULL;
            if (eGeoFieldtype != Geom.GetType())
            {
                switch (eGeoFieldtype)
                {
                case wkbLineString:
                    pNewGeom = OGRGeometryFactory::forceToLineString(Geom.Copy());
                    break;
                case wkbPolygon:
                    pNewGeom = OGRGeometryFactory::forceToPolygon(Geom.Copy());
                    break;
                case wkbMultiPoint:
                    pNewGeom = OGRGeometryFactory::forceToMultiPoint(Geom.Copy());
                    break;
                case wkbMultiLineString:
                    pNewGeom = OGRGeometryFactory::forceToMultiLineString(Geom.Copy());
                    break;
                case wkbMultiPolygon:
                    pNewGeom = OGRGeometryFactory::forceToMultiPolygon(Geom.Copy());
                    break;
                case wkbPoint:
                default:
                    pNewGeom = Geom.Copy();
                    break;
                };
                
            }
            else
            {
                pNewGeom = Geom.Copy();
            }

            if (NULL != poCT)
            {
                if (wkbFlatten(Geom.GetType()) != wkbUnknown)
                {                    
                    OGRErr eErr = pNewGeom->transform(poCT);
                    if (eErr != OGRERR_NONE)
                    {
                        pTrackCancel->PutMessage(wxString::Format(_("Geometry transform failed\nFeature id %d"), Feature.GetFID()), wxNOT_FOUND, enumGISMessageWarning);
                    }
                }
            }
            newFeature.SetGeometryDirectly(wxGISGeometry(pNewGeom, false));
        }
        
        if (pDstDataSet->GetSubType() == enumVecDXF)
        {
            wxString sFieldText;
            //    //LABEL(f:"Arial, Helvetica", s:12pt, t:"Hello World!")
            for (size_t i = 0; i < staFieldMap.size(); ++i)
            {
                sFieldText += Feature.GetFieldAsString(staFieldMap[i].nSrsFieldNo);
                sFieldText.Append(wxT("\n"));
            }
            wxString sStyleLabel = wxString::Format(wxT("LABEL(f:\"Arial, Helvetica\", s:12pt, t:\"%s\")"), sFieldText.c_str());
            newFeature.SetStyleString(sStyleLabel);
        }
        else
        {
            for ( i = 0; i < staFieldMap.size(); ++i)
            {
                switch (staFieldMap[i].eFieldType)
                {
                 case OFTRealList:
                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDoubleList(staFieldMap[i].nSrsFieldNo));
                    break;
                case OFTIntegerList:
                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsIntegerList(staFieldMap[i].nSrsFieldNo));
                    break;
                case OFTStringList:
                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsStringList(staFieldMap[i].nSrsFieldNo));
                    break;
                case OFTString:
#ifdef CPL_RECODE_ICONV
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsString(staFieldMap[i].nSrsFieldNo));
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsChar(staFieldMap[i].nSrsFieldNo));
//                    break;
#else
                    if (bFastConv)
                    {
                        const char* pszStr = Feature.GetFieldAsChar(staFieldMap[i].nSrsFieldNo);
                        if (oEncConverter.Convert(pszStr, szMaxStr))
                        {
                            newFeature.SetField(staFieldMap[i].nDstFieldNo, szMaxStr);
                            break;
                        }
                    }
#endif //CPL_RECODE_ICONV
                case OFTDate:
                case OFTTime:
                case OFTDateTime:
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDateTime(staFieldMap[i].nSrsFieldNo));
//                    break;
                case OFTReal:
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsDouble(staFieldMap[i].nSrsFieldNo));
//                    break;
                case OFTInteger:
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsInteger(staFieldMap[i].nSrsFieldNo));
//                    break;
                default:
                    //TODO: recode to output encoding if pDstDataSet encoding diffes from src
                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetRawField(staFieldMap[i].nSrsFieldNo));
//                    newFeature.SetField(staFieldMap[i].nDstFieldNo, Feature.GetFieldAsString(staFieldMap[i].nSrsFieldNo));
                    break;
                };
            }
        }

        OGRErr eErr = pDstDataSet->StoreFeature(newFeature);
        if(eErr != OGRERR_NONE)
        {
            wxString sErr = wxString::Format(_("Error create feature!\nSource feature FID:%d\nOGR error: "), Feature.GetFID());
            CPLString sFullErr(sErr.mb_str(wxConvUTF8));
            sFullErr += CPLGetLastErrorMsg();
            CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
            }
        }

        if (pProgressor)
        {
            pProgressor->SetValue(nCounter++);
        }
    }

    if (NULL != poCT)
    {
        OCTDestroyCoordinateTransformation(poCT);
    }

    return true;
}

bool ExportFormatEx(wxGISTable* const pSrsDataSet, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, const wxGISSpatialFilter &SpaFilter, OGRFeatureDefn* const poFields, char ** papszDataSourceOptions, char ** papszLayerOptions, bool bCreateEmpty, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(NULL != pSrsDataSet && NULL != pFilter && NULL != poFields, false, wxT("Input data are invalid"));

    if (pTrackCancel)
    {
        if (SpaFilter.GetWhereClause().IsEmpty())
        {
            pTrackCancel->PutMessage(wxString::Format(_("Start copy records from '%s' to '%s'"), pSrsDataSet->GetName().c_str(), sName.c_str()), wxNOT_FOUND, enumGISMessageNorm);
        }
        else
        {
            pTrackCancel->PutMessage(wxString::Format(_("Start copy records from '%s' to '%s'\nWHERE %s"), pSrsDataSet->GetName().c_str(), sName.c_str(), SpaFilter.GetWhereClause().c_str()), wxNOT_FOUND, enumGISMessageNorm);
        }
    }

    if (pSrsDataSet->GetFeatureCount(false, pTrackCancel) == 0 && !bCreateEmpty)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("No records to export"), wxNOT_FOUND, enumGISMessageNorm);
        }

        return true;
    }

    wxGISTable* pDstDataSet = wxDynamicCast(CreateDataset(sPath, sName, pFilter, poFields, wxNullSpatialReference, papszDataSourceOptions, papszLayerOptions, pTrackCancel), wxGISTable);
    if (NULL == pDstDataSet)
    {
        wxString sErr(_("Error creating new dataset!\nOGR error: "));
        CPLString sFullErr(sErr.mb_str(wxConvUTF8));
        sFullErr += CPLGetLastErrorMsg();
        CPLError(CE_Failure, CPLE_AppDefined, sFullErr);
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }


    OGRErr eErr = pDstDataSet->StartTransaction();

    //copy data
    if (!CopyRows(pSrsDataSet, pDstDataSet, pTrackCancel))
    {
        wxString sErr(_("Error copying data to a new dataset!\nOGR error: "));
        CPLString sFullErr(sErr.mb_str(wxConvUTF8));
        sFullErr += CPLGetLastErrorMsg();
        CPLError(CE_Failure, CPLE_FileIO, sFullErr);
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
        }

        //remove filter
        pSrsDataSet->SetFilter();
        eErr = pDstDataSet->RollbackTransaction();
        return false;
    }

    eErr = pDstDataSet->CommitTransaction();

    pDstDataSet->Close();

    return eErr == OGRERR_NONE;
}

bool ExportFormatEx(wxGISFeatureDataset* const pSrsDataSet, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, const wxGISSpatialFilter &SpaFilter, OGRFeatureDefn* const poFields, const wxGISSpatialReference &oSpatialRef, char ** papszDataSourceOptions, char ** papszLayerOptions, bool bCreateEmpty, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(NULL != pSrsDataSet && NULL != pFilter && NULL != poFields, false, wxT("Input data are invalid"));

    if (pTrackCancel)
    {
        if (SpaFilter.GetWhereClause().IsEmpty())
        {
            pTrackCancel->PutMessage(wxString::Format(_("Start copy records from '%s' to '%s'"), pSrsDataSet->GetName().c_str(), sName.c_str()), wxNOT_FOUND, enumGISMessageNorm);
        }
        else
        {
            pTrackCancel->PutMessage(wxString::Format(_("Start copy records from '%s' to '%s'\nWHERE %s"), pSrsDataSet->GetName().c_str(), sName.c_str(), SpaFilter.GetWhereClause().c_str()), wxNOT_FOUND, enumGISMessageNorm);
        }
    }

    //set filter
    pSrsDataSet->SetFilter(SpaFilter);

    if (pSrsDataSet->GetFeatureCount(true, pTrackCancel) == 0 && !bCreateEmpty)
    {
        pSrsDataSet->SetFilter();
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("No records to export"), wxNOT_FOUND, enumGISMessageNorm);
        }

        return true;
    }

    wxGISFeatureDataset* pDstDataSet = wxDynamicCast(CreateDataset(sPath, sName, pFilter, poFields, oSpatialRef, papszDataSourceOptions, papszLayerOptions, pTrackCancel), wxGISFeatureDataset);
    if (NULL == pDstDataSet)
    {
        wxString sErr(_("Error creating new dataset!\nOGR error: "));
        CPLString sFullErr(sErr.mb_str(wxConvUTF8));
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }


    OGRErr eErr = pDstDataSet->StartTransaction();

    //copy data
    if (!CopyRows(pSrsDataSet, pDstDataSet, pTrackCancel))
    {
        wxString sErr(_("Error copying data to a new dataset!\nOGR error: "));
        CPLString sFullErr(sErr.mb_str(wxConvUTF8));
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
        }

        //remove filter
        pSrsDataSet->SetFilter();
        eErr = pDstDataSet->RollbackTransaction();
        return false;
    }

    //remove filter
    pSrsDataSet->SetFilter();

    eErr = pDstDataSet->CommitTransaction();

    pDstDataSet->Close();

    return eErr == OGRERR_NONE;
}

bool ExportFormat(wxGISTable* const pSrsDataSet, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, const wxGISSpatialFilter &SpaFilter, char ** papszDataSourceOptions, char ** papszLayerOptions, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(NULL != pSrsDataSet && NULL != pFilter, false, wxT("Input data are invalid"));
    wxString sExt = pFilter->GetExt();

    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(wxString::Format(_("Exporting %s to %s"), pSrsDataSet->GetName().c_str(), wxString(sName + wxT(".") + sExt).c_str()), wxNOT_FOUND, enumGISMessageTitle);
    }

    OGRFeatureDefn *pDef = pSrsDataSet->GetDefinition();
    if (NULL == pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(_("Get features..."), wxNOT_FOUND, enumGISMessageNorm);
    }

    CPLString szBaseName = (char*)CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    wxGxObject* pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
    if (!OverWriteGxObject(pObj, pTrackCancel))
    {
        wxString sErr(_("Overwrite failed"));
        CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    wxGISConfigOptionReset reset_copy("PG_USE_COPY", "YES", CPLGetConfigOption("PG_USE_COPY", "YES"));

    if (!ExportFormatEx(pSrsDataSet, sPath, sName, pFilter, SpaFilter, pDef, papszDataSourceOptions, papszLayerOptions, true, pTrackCancel))
    {
        return false;
    }
    return true;
}

bool ExportFormat(wxGISFeatureDataset* const pSrsDataSet, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, const wxGISSpatialFilter &SpaFilter, char ** papszDataSourceOptions, char ** papszLayerOptions, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(NULL != pSrsDataSet && NULL != pFilter, false, wxT("Input data are invalid"));

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if (pTrackCancel)
    {
        pTrackCancel->PutMessage(wxString::Format(_("Exporting %s to %s"), pSrsDataSet->GetName().c_str(), wxString(sName + wxT(".") + sExt).c_str()), wxNOT_FOUND, enumGISMessageTitle);
    }


    const wxGISSpatialReference SrcSpaRef = pSrsDataSet->GetSpatialReference();
    if (!SrcSpaRef.IsOk() && (nNewSubType == enumVecKML || nNewSubType == enumVecKMZ))
    {
        wxString sErr(_("Input spatial reference is not defined!"));
        CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    wxGISSpatialReference DstSpaRef;
    if (nNewSubType == enumVecKML || nNewSubType == enumVecKMZ)
    {
        DstSpaRef = wxGISSpatialReference(new OGRSpatialReference(SRS_WKT_WGS84));
    }
    else if (SrcSpaRef.IsOk())
    {
        DstSpaRef = wxGISSpatialReference(SrcSpaRef.Clone());
    }

    OGRFeatureDefn *pDef = pSrsDataSet->GetDefinition();
    if(NULL == pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

     //check multi geometry
    OGRwkbGeometryType eGeomType = pSrsDataSet->GetGeometryType();

    bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(eGeomType) == wkbUnknown || wkbFlatten(eGeomType) == wkbGeometryCollection);
    wxString sWhereClause = SpaFilter.GetWhereClause();

	bool bExported(false);

    //size_t nFullCount = pDSet->GetFeatureCount(false, pTrackCancel);
    wxGISSpatialFilter oNewSpaFilter(SpaFilter);
    if(bIsMultigeom)
    {
        //export points
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='POINT'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='POINT'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("POINT")), wxNOT_FOUND, enumGISMessageNorm);
        }

        //delete exist
        
        CPLString szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_point"))).mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        wxGxObject* pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }


        OGRFeatureDefn *pNewDef = pDef->Clone();
        pNewDef->SetGeomType( wkbPoint );
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_point")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }

        //export polygons
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='POLYGON'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='POLYGON'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("POLYGON")), wxNOT_FOUND, enumGISMessageNorm);
        }

        szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_polygon"))).mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }


        pNewDef = pDef->Clone();
        pNewDef->SetGeomType(wkbPolygon);
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_polygon")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }

        //export linestrings
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='LINESTRING'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='LINESTRING'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("LINESTRING")), wxNOT_FOUND, enumGISMessageNorm);
        }

        szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_line"))).mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        pNewDef = pDef->Clone();
        pNewDef->SetGeomType(wkbLineString);
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_line")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }

        //export multipoint
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='MULTIPOINT'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='MULTIPOINT'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("MULTIPOINT")), wxNOT_FOUND, enumGISMessageNorm);
        }

        szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_mpoint"))).ToUTF8(), sExt.ToUTF8());
        pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        pNewDef = pDef->Clone();
        pNewDef->SetGeomType(wkbMultiPoint);
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_mpoint")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }

        //export multilinestring
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='MULTILINESTRING'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='MULTILINESTRING'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("MULTILINESTRING")), wxNOT_FOUND, enumGISMessageNorm);
        }

        szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_mline"))).mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        pNewDef = pDef->Clone();
        pNewDef->SetGeomType(wkbMultiLineString);
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_mline")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }

        //export multilinestring
        if (sWhereClause.IsEmpty())
        {
            oNewSpaFilter.SetWhereClause(wxT("OGR_GEOMETRY='MULTIPOLYGON'"));
        }
        else
        {
            oNewSpaFilter.SetWhereClause(sWhereClause + wxT(" AND OGR_GEOMETRY='MULTIPOLYGON'"));
        }

        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("Get '%s' features"), wxT("MULTIPOLYGON")), wxNOT_FOUND, enumGISMessageNorm);
        }

        szBaseName = (char*)CPLFormFilename(sPath, wxString(sName + wxString(_("_mpolygon"))).mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        pNewDef = pDef->Clone();
        pNewDef->SetGeomType(wkbMultiPolygon);
        if (!ExportFormatEx(pSrsDataSet, sPath, sName + wxString(_("_mpolygon")), pFilter, oNewSpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, false, pTrackCancel))
        {
            return false;
        }
    }
    else //not multigeom
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(_("Get features..."), wxNOT_FOUND, enumGISMessageNorm);
        }

        CPLString szBaseName = (char*)CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
        wxGxObject* pObj = wxDynamicCast(GetGxCatalog()->FindGxObjectByPath(wxString(szBaseName, wxConvUTF8)), wxGxObject);
        if (!OverWriteGxObject(pObj, pTrackCancel))
        {
            wxString sErr(_("Overwrite failed"));
            CPLError(CE_Failure, CPLE_AppDefined, sErr.mb_str(wxConvUTF8));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        //check if data have multi types
        bool bIsMulti = false;
        OGRwkbGeometryType eCheckType;
        OGRFeatureDefn *pNewDef = pDef;


        if (eGeomType > 1 && eGeomType < 4 && nNewSubType == enumVecPostGIS)
        {
            //check to multi
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString::Format(_("Check if features geometry is the type of %s"), OGRGeometryTypeToName(pNewDef->GetGeomType())), wxNOT_FOUND, enumGISMessageInfo);
            }

            bool bToMulti = false;

            wxArrayString saIgnoredFields = pSrsDataSet->GetFieldNames();
            saIgnoredFields.Add(wxT("OGR_STYLE"));
            pSrsDataSet->SetIgnoredFields(saIgnoredFields);
            pSrsDataSet->Reset();


            wxGISFeature Feature;
            while ((Feature = pSrsDataSet->Next()).IsOk())
            {
                //check if Feature will destroy by Ref Count
                wxGISGeometry Geom = Feature.GetGeometry();
                if (Geom.IsOk())
                {
                    if (Geom.GetType() > eGeomType)
                    {
                        bToMulti = true;
                        break;
                    }
                }
            }

            if (bToMulti)
            {
                pNewDef = pDef->Clone();
                pNewDef->SetGeomType((OGRwkbGeometryType)(eGeomType + 3));//set multi
                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(wxString::Format(_("Force geometry field to %s"), OGRGeometryTypeToName(pNewDef->GetGeomType())), wxNOT_FOUND, enumGISMessageInfo);
                }
            }
            saIgnoredFields.Clear();
            pSrsDataSet->SetIgnoredFields(saIgnoredFields);
        }

        //TODO:
        //if (nNewSubType == enumVecPostGIS && pSrsDataSet->GetFeatureCount() > MAX_FEATURES_FORINSERT)
        //{
        wxGISConfigOptionReset reset_copy("PG_USE_COPY", "YES", CPLGetConfigOption("PG_USE_COPY", "YES"));
        //}


        if (!ExportFormatEx(pSrsDataSet, sPath, sName, pFilter, SpaFilter, pNewDef, DstSpaRef, papszDataSourceOptions, papszLayerOptions, true, pTrackCancel))
        {
            return false;
        }
    }
    return true;
}

/*
bool Project(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRSpatialReference* pNewSpaRef, ITrackCancel* pTrackCancel)
{
    CPLErrorReset();

    if(!pFilter || !pDSet)
        return false;

    int nNewSubType = pFilter->GetSubType();
    //check multi geometry
    OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
    bool bIsMultigeom = (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
    if(bIsMultigeom && nNewSubType == enumVecESRIShapefile)
    {
        wxString sErr(_("Input feature class has multi geometry but output doesn't support it!"));
        CPLString sFullErr(sErr.mb_str());
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Projecting %s to %s"), pDSet->GetName().c_str(), sName.c_str()), wxNOT_FOUND, enumGISMessageTitle);

    const OGRSpatialReferenceSPtr pSrcSpaRef = pDSet->GetSpatialReference();
    if(!pSrcSpaRef)
    {
        wxString sErr(_("Input spatial reference is not defined! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    if(!pNewSpaRef)
    {
        wxString sErr(_("Output spatial reference is not defined! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    OGRFeatureDefn *pDef = pDSet->GetDefinition();
    if(!pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    OGRCoordinateTransformation *poCT(NULL);
    if(pSrcSpaRef->IsSame(pNewSpaRef))
    {
        wxString sErr(_("The Spatial references are same! Nothing project"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    poCT = OGRCreateCoordinateTransformation( pSrcSpaRef.get(), pNewSpaRef );
    if(!poCT)
    {
        wxString sErr(_("The unknown transformation! OGR Error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
    if(!pNewDSet)
    {
        wxString sErr(_("Error creating new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }


/*    //get limits
    OGRPolygon* pRgn1 = NULL;
    OGRPolygon* pRgn2 = NULL;

    wxGISSpatialReferenceValidator GISSpaRefValidator;
    wxString sProjName;
    if(pNewSpaRef->IsProjected())
        sProjName = wgMB2WX(pNewSpaRef->GetAttrValue("PROJCS"));
    else
        sProjName = wgMB2WX(pNewSpaRef->GetAttrValue("GEOGCS"));
    if(GISSpaRefValidator.IsLimitsSet(sProjName))
    {
        LIMITS lims = GISSpaRefValidator.GetLimits(sProjName);
        if(lims.minx > lims.maxx)
        {
            OGRLinearRing ring1;
            ring1.addPoint(lims.minx,lims.miny);
            ring1.addPoint(lims.minx,lims.maxy);
            ring1.addPoint(180.0,lims.maxy);
            ring1.addPoint(180.0,lims.miny);
            ring1.closeRings();

            pRgn1 = new OGRPolygon();
            pRgn1->addRing(&ring1);
            pRgn1->flattenTo2D();

            OGRLinearRing ring2;
            ring2.addPoint(-180.0,lims.miny);
            ring2.addPoint(-180.0,lims.maxy);
            ring2.addPoint(lims.maxx,lims.maxy);
            ring2.addPoint(lims.maxx,lims.miny);
            ring2.closeRings();

            pRgn2 = new OGRPolygon();
            pRgn2->addRing(&ring2);
            pRgn2->flattenTo2D();
        }
        else
        {
            OGRLinearRing ring;
            ring.addPoint(lims.minx,lims.miny);
            ring.addPoint(lims.minx,lims.maxy);
            ring.addPoint(lims.maxx,lims.maxy);
            ring.addPoint(lims.maxx,lims.miny);
            ring.closeRings();

            pRgn1 = new OGRPolygon();
            pRgn1->addRing(&ring);
            pRgn1->flattenTo2D();
        }
        //WGS84
        OGRSpatialReference* pWGSSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);

        if(pRgn1 != NULL)
        {
            pRgn1->assignSpatialReference(pWGSSpaRef);
            pRgn1->segmentize(SEGSTEP);
        }
        if(pRgn2 != NULL)
        {
            pRgn2->assignSpatialReference(pWGSSpaRef);
            pRgn2->segmentize(SEGSTEP);
        }
        pWGSSpaRef->Dereference();

        if(!pSrcSpaRef->IsSame(pWGSSpaRef))
        {
            if(pRgn1 && pRgn1->transformTo(pSrcSpaRef.get()) != OGRERR_NONE)
                wxDELETE(pRgn1);
            if(pRgn2 && pRgn2->transformTo(pSrcSpaRef.get()) != OGRERR_NONE)
                wxDELETE(pRgn2);
        }
    }

    OGREnvelope *pRgnEnv1(NULL), *pRgnEnv2(NULL);
    if(pRgn1)
    {
        pRgnEnv1 = new OGREnvelope();
        pRgn1->getEnvelope(pRgnEnv1);
    }
    if(pRgn2)
    {
        pRgnEnv2 = new OGREnvelope();
        pRgn2->getEnvelope(pRgnEnv2);
    }
*/
/*
    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString(_("Start projecting")), wxNOT_FOUND, enumGISMessageNorm);
    }

    int nCounter(0);
    if(pProgressor)
        pProgressor->SetRange(pDSet->GetFeatureCount());

    pDSet->Reset();
    OGRFeatureSPtr pFeature;
    while((pFeature = pDSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.mb_str());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
            return false;
        }

        OGRGeometry *pGeom = pFeature->GetGeometryRef()->clone();
        if(pGeom)
        {
            //if(pRgn1 == NULL && pRgn2 == NULL)
            //{
                if(pGeom->transform( poCT ) != OGRERR_NONE)
                {
                    pTrackCancel->PutMessage(wxString::Format(_("Error project feature #%d"), pFeature->GetFID()), wxNOT_FOUND, enumGISMessageWarning);
                    wxDELETE(pGeom);
                    continue;
                }
                else
                    pFeature->SetGeometryDirectly(pGeom);
            //}
            //else
            //{
            //    OGRGeometry* pCutGeom = CheckRgnAndTransform(pGeom, pRgn1, pRgn2, pRgnEnv1, pRgnEnv2, poCT);
            //    if(pCutGeom)
            //        pFeature->SetGeometryDirectly(pCutGeom);
            //    else
            //    {
            //        pTrackCancel->PutMessage(wxString::Format(_("Error project feature #%d"), pFeature->GetFID()), wxNOT_FOUND, enumGISMessageWarning);
            //        wxDELETE(pGeom);
            //        continue;
            //    }
            //}
        }

        //////////////////////
        //wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();

        //OGRFeatureDefn *pFeatureDefn = pFeature->GetDefnRef();
        //if(!pFeatureDefn)
        //{
        //    OGRFeature::DestroyFeature(pFeature);
        //    wxDELETE(pGeom);
        //    continue;
        //}
        //
        //CPLErrorReset();
        //wxString sACText;

        //for(size_t i = 0; i < pFeatureDefn->GetFieldCount(); ++i)
        //{
        //    OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
        //    if(pFieldDefn)
        //    {
        //        OGRFieldType nType = pFieldDefn->GetType();
        //        if(OFTString == nType)
        //        {
        //            wxString sFieldString;
        //            if(CPLIsUTF8(pFeature->GetFieldAsString(i), -1))
        //            {
        //                sFieldString = wxString::FromUTF8(pFeature->GetFieldAsString(i));
        //            }
        //            else
        //            {
        //                sFieldString = wxString(wgMB2WX(pFeature->GetFieldAsString(i)));
        //            }
        //            //wxCSConv inconv(OutputEncoding);
        //            //CPLString cplstr(pFeature->GetFieldAsString(i));
        //            //wxString sFieldString(cplstr.
        //            ////wxString sFieldString(wgMB2WX(pFeature->GetFieldAsString(i)));
        //            //wxCSConv outconv(OutputEncoding);
        //            //pFeature->SetField(i, sFieldString.mb_str(outconv));

        //            if(OutputEncoding == wxFONTENCODING_DEFAULT)
        //                pFeature->SetField(i, wgWX2MB(sFieldString));
        //            else
        //            {
        //                wxCSConv outconv(OutputEncoding);
        //                pFeature->SetField(i, sFieldString.mb_str(outconv));
        //            }
        //            if(pDstDataSet->GetSubType() == enumVecDXF)
        //                sACText += sFieldString + wxT("\n");
        //        }
        //        //TODO: OFTStringList
        //    }
        //}
        ////////////////

        OGRErr eErr = pNewDSet->StoreFeature(pFeature);
        if(eErr != OGRERR_NONE)
        {
            wxString sErr(_("Error create feature! OGR error: "));
            CPLString sFullErr(sErr.mb_str());
            sFullErr += CPLGetLastErrorMsg();
            CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        }
        nCounter++;
        if(pProgressor)
            pProgressor->SetValue(nCounter);
    }

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

    //wxDELETE(pRgnEnv1);
    //wxDELETE(pRgnEnv2);
    //wxDELETE(pRgn1);
    //wxDELETE(pRgn2);

    return true;
}

OGRGeometry* CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT)
{
    if(!pFeatureGeom)
        return NULL;
    if(!poCT)
        return NULL;

    OGREnvelope FeatureEnv;
    OGRGeometry *pGeom(NULL), *pGeom1(NULL), *pGeom2(NULL);

    pFeatureGeom->getEnvelope(&FeatureEnv);

    if(pRgnEnv1 && FeatureEnv.Intersects(*pRgnEnv1))
    {
        if(pRgnEnv1->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn1))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom1 = Intersection(pFeatureGeom, pRgn1, pRgnEnv1);
    }
    if(pRgnEnv2 && FeatureEnv.Intersects(*pRgnEnv2))
    {
        if(pRgnEnv2->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn2))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom2= Intersection(pFeatureGeom, pRgn2, pRgnEnv2);
    }

    if(pGeom1 && !pGeom2)
        pGeom = pGeom1;
    else if(pGeom2 && !pGeom1)
        pGeom = pGeom2;
    else if(pGeom1 && pGeom2)
    {
        OGRGeometryCollection* pGeometryCollection = new OGRGeometryCollection();
        pGeometryCollection->addGeometryDirectly(pGeom1);
        pGeometryCollection->addGeometryDirectly(pGeom2);
        pGeom = pGeometryCollection;
    }


PROCEED:
    if(!pGeom)
        return NULL;

    if(pGeom->getSpatialReference() == NULL)
        pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());

    if(pGeom->transform( poCT ) != OGRERR_NONE)
    {
        wxDELETE(pGeom);
        return NULL;
    }
    return pGeom;
}

OGRGeometry* Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv)
{
    if(!pFeatureGeom || !pRgnEnv)
        return NULL;

    OGREnvelope FeatureEnv;
    pFeatureGeom->getEnvelope(&FeatureEnv);

    OGRwkbGeometryType gType = wkbFlatten(pFeatureGeom->getGeometryType());
    switch(gType)
    {
    case wkbPoint:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return NULL;
    case wkbLineString:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
#ifdef FAST_BISHOP_INTERSECTION
        {
            wxGISAlgorithm alg;
            return alg.FastLineIntersection(pFeatureGeom, pRgn);
        }
#else
        return pFeatureGeom->Intersection(pRgn);
#endif
    case wkbPolygon:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
#ifdef FAST_BISHOP_INTERSECTION
        {
            wxGISAlgorithm alg;
            return alg.FastPolyIntersection(pFeatureGeom, pRgn);
        }
#else
        return pFeatureGeom->Intersection(pRgn);
#endif
    case wkbLinearRing:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return pFeatureGeom->Intersection(pRgn);
    case wkbMultiPoint:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiPoint();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbMultiLineString:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiLineString();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbMultiPolygon:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiPolygon();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbGeometryCollection:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbUnknown:
    case wkbNone:
        break;
    }
    return NULL;
}

bool GeometryVerticesToPoints(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Convert geometry vertices to point. Source dataset %s. Destination dataset %s"), pDSet->GetName().c_str(), sName.c_str()), wxNOT_FOUND, enumGISMessageTitle);

    const OGRSpatialReferenceSPtr pSrcSpaRef = pDSet->GetSpatialReference();
    if(!pSrcSpaRef)
    {
        wxString sErr(_("Input spatial reference is not defined! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    OGRSpatialReference* pNewSpaRef(NULL);
    if(nNewSubType == enumVecKML || nNewSubType == enumVecKMZ)
        pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
    else
        pNewSpaRef = pSrcSpaRef->Clone();

    //OGRFeatureDefn *pDef = pDSet->GetDefiniton();
    //if(!pDef)
    //{
    //    wxString sErr(_("Error read dataset definition"));
    //    CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
    //    return false;
    //}

    OGRFeatureDefn *pNewDef = new OGRFeatureDefn("vertices");
	pNewDef->AddFieldDefn( new OGRFieldDefn("ogc_fid", OFTInteger) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("geom_id", OFTInteger) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("distance", OFTReal ) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("lat", OFTReal ) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("lon", OFTReal ) );
	pNewDef->AddFieldDefn( new OGRFieldDefn("elevation", OFTReal ) );
    pNewDef->SetGeomType( wkbPoint25D );


    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pNewDef, pNewSpaRef);
    if(!pNewDSet)
    {
        wxString sErr(_("Error creating new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    OGRCoordinateTransformation *poCT(NULL);
    if(!pSrcSpaRef->IsSame(pNewSpaRef))
    {
		poCT = OGRCreateCoordinateTransformation( pSrcSpaRef.get(), pNewSpaRef );
		if(!poCT)
		{
			wxString sErr(_("The unknown transformation! OGR Error: "));
			CPLString sFullErr(sErr.mb_str());
			sFullErr += CPLGetLastErrorMsg();
			CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
			return false;
		}
	}

	int nCounter(0);
	long nFidCounter(0);
 	OGRFeatureSPtr poFeature;
	pDSet->Reset();
    while((poFeature = pDSet->Next()) != NULL)
    {
		nCounter++;

        if(pTrackCancel)
        {
			if(!pTrackCancel->Continue())
				return false;
			pTrackCancel->PutMessage(wxString::Format(_("Proceed No. %d Geometry"), nCounter), wxNOT_FOUND, enumGISMessageNorm);
        }


        OGRGeometry* pGeom = poFeature->StealGeometry();//GetGeometryRef();
        if(!pGeom)
            continue;

		if(!GeometryVerticesToPointsDataset(poFeature->GetFID(), pGeom, pNewDSet, poCT, nFidCounter, pTrackCancel))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString(_("Unsupported geometry type")), wxNOT_FOUND, enumGISMessageWarning);
		}
		else
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString::Format(_("Geometry No. %d added"), nCounter), wxNOT_FOUND, enumGISMessageInfo);
		}
		wxDELETE(pGeom);
	}

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);
	else
		wxDELETE(pNewSpaRef);
	return true;
}

bool GeometryVerticesToPointsDataset(long nGeomFID, OGRGeometry* pGeom, wxGISFeatureDatasetSPtr pDSet, OGRCoordinateTransformation *poCT, long &nFidCounter, ITrackCancel* pTrackCancel)
{
    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
		pProgressor = pTrackCancel->GetProgressor();

	OGRwkbGeometryType nType = pGeom->getGeometryType();
	switch(nType)
	{
	case wkbLineString:
	case wkbLineString25D:
		{
			OGRLineString* pLineString = (OGRLineString*)pGeom;
			int nCount = pLineString->getNumPoints();
			if(pProgressor)
				pProgressor->SetRange(nCount);

			OGRPoint* pPrevPt(NULL);
			double dDist(0);

			for(size_t i = 0; i < nCount; ++i)
			{
				OGRPoint Point;
				pLineString->getPoint(i, &Point);
				double dX = Point.getX();
				double dY = Point.getY();
				double dZ = Point.getZ();
				if(poCT)
					poCT->Transform(1, &dX, &dY, &dZ);

				//OGRFeatureDefn* pFDefn =  pDSet->GetDefinition();
				//OGRFeature* pFeature = OGRFeature::CreateFeature( pFDefn );
				OGRFeatureSPtr pFeature = pDSet->CreateFeature();
				if(pFeature)
				{
					pFeature->SetField(0, (int)nFidCounter); //geom_id
					nFidCounter++;
					pFeature->SetField(1, (int)nGeomFID); //geom_id

					OGRPoint* pPt = new OGRPoint(dX, dY, dZ);
					pPt->assignSpatialReference(pDSet->GetSpatialReference().get());
					pPt->setCoordinateDimension(pLineString->getCoordinateDimension());

					if(pPrevPt)
						dDist += pPrevPt->Distance(pPt);
					pFeature->SetField(2, dDist); //distance
					pFeature->SetField(3, dY); //lat
					pFeature->SetField(4, dX); //lon
					pFeature->SetField(5, dZ); //elevation

					pFeature->SetGeometryDirectly( (OGRGeometry*)pPt );

					pPrevPt = pPt;

					if(pDSet->StoreFeature(pFeature) != OGRERR_NONE)
						if(pTrackCancel)
							pTrackCancel->PutMessage(wxString::Format(_("Failed add point No. %d"), i), wxNOT_FOUND, enumGISMessageWarning);
				}

				if(pProgressor)
					pProgressor->SetValue(i);
				if(pTrackCancel && !pTrackCancel->Continue())
					return false;
			}

			return true;
		}
		break;
	case wkbPoint:
	case wkbPoint25D:
	case wkbMultiPoint:
	case wkbMultiPoint25D:
	case wkbMultiLineString:
	case wkbMultiLineString25D:
	case wkbGeometryCollection:
	case wkbGeometryCollection25D:
	default://wkbUnknown wkbPolygon wkbMultiPolygon wkbNone wkbLinearRing wkbPolygon25D wkbMultiPolygon25D
		return false;
	}
	return true;
}
*/
wxGISDataset* CreateDataset(const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, OGRFeatureDefn* const poFields, const wxGISSpatialReference &oSpatialRef, char ** papszDataSourceOptions, char ** papszLayerOptions, ITrackCancel* const pTrackCancel)
{

    bool bIsFilerValid = pFilter && (pFilter->GetType() == enumGISFeatureDataset || pFilter->GetType() == enumGISTableDataset);
    if(!bIsFilerValid)
    {
        CPLError( CE_Failure, CPLE_FileIO, "Input data is invalid: unsuported filter" );
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Input data is invalid: unsuported filter"), wxNOT_FOUND, enumGISMessageErr);
        return NULL;
    }

    CPLErrorReset();

	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pFilter->GetDriver().mb_str() );
    if(poDriver == NULL)
    {
        wxString sErr = wxString::Format(_("The driver '%s' is not available!\nOGR error: "), pFilter->GetDriver().c_str());
        CPLString sFullErr(sErr.ToUTF8());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
        }
        return NULL;
    }

    CPLString szFullPath = sPath;
    if (!sName.IsEmpty())
    {
        szFullPath  = CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), pFilter->GetExt().mb_str(wxConvUTF8));
    }

    OGRDataSource *poDS = NULL;
    if ((pFilter->GetType() == enumGISFeatureDataset && pFilter->GetSubType() == enumVecPostGIS) || (pFilter->GetType() == enumGISTableDataset && pFilter->GetSubType() == enumTablePostgres))
    {
#ifdef wxGIS_USE_POSTGRES

        CPLString sDSPath = CPLGetPath(sPath);

        wxGISPostgresDataSource* pDataSource = new wxGISPostgresDataSource(sDSPath);
        if (!pDataSource->Open(TRUE))
        {
            wxString sErr(_("Datasource open failed! OGR error: "));
            CPLString sFullErr(sErr.mb_str(wxConvUTF8));
            sFullErr += CPLGetLastErrorMsg();
            CPLError(CE_Failure, CPLE_FileIO, sFullErr);
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
            }
            return NULL;
        }
        //get dataset from path
        poDS = pDataSource->GetDataSourceRef();
        poDS->Reference();

        wxFontEncoding eEnc = pDataSource->GetEncoding();
        wxString sEnc = wxFontMapperBase::GetEncodingName(eEnc);

        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "ENCODING", sEnc.mb_str());

        wxDELETE(pDataSource);

        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "LAUNDER", "NO");


#endif // wxGIS_USE_POSTGRES
    }
    else
    {
        poDS = poDriver->CreateDataSource(szFullPath, papszDataSourceOptions);
    }

    if(poDS == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output file '%s'! OGR error: "), wxString(sPath, wxConvUTF8).c_str());
        CPLString sFullErr(sErr.ToUTF8());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
        }
        return NULL;
    }

    wxString sNewName = sName;
    if(sNewName.IsEmpty())
    {
        //create layer name from file name
        sNewName = wxString(CPLGetBasename(sPath), wxConvUTF8);
    }

    sNewName.Replace(wxT("."), wxT("_"));
    sNewName.Replace(wxT(" "), wxT("_"));
    sNewName.Replace(wxT("&"), wxT("_"));
    //sNewName.Truncate(27);

	if(wxIsdigit(sNewName.GetChar(0)))
		sNewName.Prepend(_("Layer_"));

    CPLString szName;
    if(pFilter->GetType() == enumGISFeatureDataset)
    {
        if(pFilter->GetSubType() == enumVecKMZ)
		    szName = Transliterate(CPLString(sNewName.mb_str()).c_str());
        else if (pFilter->GetSubType() == enumVecKML)
            szName = CPLString(sNewName.ToUTF8());//wxCSConv(wxT("cp-866"))));
        else if (pFilter->GetSubType() == enumVecPostGIS)
            szName = CPLGetFilename(sPath) + CPLString(".") + CPLString(sNewName.ToUTF8());
        else
            szName = CPLString(sNewName.ToUTF8());
    }
    else
    {
        if (pFilter->GetSubType() == enumTablePostgres)
            szName = CPLGetFilename(sPath) + CPLString(".") + CPLString(sNewName.ToUTF8());
        else
            szName = CPLString(sNewName.ToUTF8());
    }

    //set systyme encoding for layer if no default set
    wxFontEncoding oEncoding;
    if (CSLFindName(papszLayerOptions, "ENCODING") == -1)
    {
        wxString sEnc = wxLocale::GetSystemEncodingName();
        const char* sz_enc = sEnc.mb_str();
        oEncoding = wxLocale::GetSystemEncoding();
        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "ENCODING", sz_enc);
    }
    else
    {
        const char* sz_enc = CSLFetchNameValue(papszLayerOptions, "ENCODING");
        oEncoding = wxFontMapperBase::GetEncodingFromName(sz_enc);
    }

#ifndef CPL_RECODE_ICONV
    wxGISConfigOptionReset reset_shp("SHAPE_ENCODING", "", CPL_ENC_ASCII);
    wxGISConfigOptionReset reset_dxf("DXF_ENCODING", "", CPL_ENC_ASCII);
#endif
    
    //TODO: test if vertical srs can storing in output format
    if (oSpatialRef.IsOk())
        oSpatialRef->StripVertical();
    OGRLayer *poLayerDest = poDS->CreateLayer(szName, oSpatialRef, poFields->GetGeomType(), papszLayerOptions);
    if(poLayerDest == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
        CPLString sFullErr(sErr.ToUTF8());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
        }
        return NULL;
    }

	CPLString szNameField, szDescField;
    if (pFilter->GetType() == enumGISFeatureDataset)
    {
        if (pFilter->GetSubType() == enumVecDXF)
        {
            wxString sErr(_("DXF layer does not support arbitrary field creation."));
            wxLogWarning(sErr);
            CPLError(CE_Warning, CPLE_AppDefined, CPLString(sErr.ToUTF8()));
            if (pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageWarning);
            }
        }
        else 
        {
            for (size_t i = 0; i < poFields->GetFieldCount(); ++i)
            {
                OGRFieldDefn *pField = poFields->GetFieldDefn(i);
                OGRFieldDefn oFieldDefn(pField);
                oFieldDefn.SetName(Transliterate(pField->GetNameRef()));
                if (pFilter->GetSubType() == enumVecKML || pFilter->GetSubType() == enumVecKMZ)
                {
                    OGRFieldType nType = pField->GetType();
                    if (OFTString == nType)
                    {
                        if (szNameField.empty())
                            szNameField = pField->GetNameRef();
                        if (szDescField.empty())
                            szDescField = pField->GetNameRef();
                    }
                }

                if (pFilter->GetSubType() == enumVecESRIShapefile && pField->GetType() == OFTTime)
                {
                    oFieldDefn.SetType(OFTString);
                    wxString sErr(_("Unsupported type for shape file - OFTTime. Change to OFTString."));
                    wxLogWarning(sErr);
                    CPLError(CE_Warning, CPLE_AppDefined, CPLString(sErr.mb_str()));
                    if (pTrackCancel)
                    {
                        pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageWarning);
                    }
                }

                if (poLayerDest->CreateField(&oFieldDefn) != OGRERR_NONE)
                {
                    wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
                    CPLString sFullErr(sErr.ToUTF8());
                    sFullErr += CPLGetLastErrorMsg();
                    CPLError(CE_Failure, CPLE_AppDefined, sFullErr);
                    if (pTrackCancel)
                    {
                        pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
                    }
                    return NULL;
                }
            }
            if (pFilter->GetSubType() == enumVecKML || pFilter->GetSubType() == enumVecKMZ)
            {
                if (!szNameField.empty())
                    CPLSetConfigOption("LIBKML_NAME_FIELD", szNameField);
                if (!szDescField.empty())
                    CPLSetConfigOption("LIBKML_DESCRIPTION_FIELD", szDescField);
                //CPLSetConfigOption( "LIBKML_TIMESTAMP_FIELD", "YES" );
            }
        }

        wxGISFeatureDataset *pFeatureDataset = new wxGISFeatureDataset(szFullPath, pFilter->GetSubType(), poLayerDest, poDS);
        pFeatureDataset->SetEncoding(oEncoding);
        return wxStaticCast(pFeatureDataset, wxGISDataset);

    }
    else if (pFilter->GetType() == enumGISTableDataset)
    {
        for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            if(pFilter->GetSubType() == enumTableDBF && pField->GetType() == OFTTime)
            {
                pField->SetType(OFTString);
                wxString sErr(_("Unsupported type for dbf file - OFTTime. Change to OFTString."));
                wxLogWarning(sErr);
                CPLError(CE_Warning, CPLE_AppDefined, CPLString(sErr.mb_str(wxConvUTF8)));
                if(pTrackCancel)
                    pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageWarning);
            }

	        if( poLayerDest->CreateField( pField ) != OGRERR_NONE )
	        {
                wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
                CPLString sFullErr(sErr.ToUTF8());
                sFullErr += CPLGetLastErrorMsg();
                CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
                if(pTrackCancel)
                    pTrackCancel->PutMessage(wxString(sFullErr, wxConvUTF8), wxNOT_FOUND, enumGISMessageErr);
                return NULL;
            }
        }

        wxGISTable *pTableDataset = new wxGISTable(sPath, pFilter->GetSubType(), poLayerDest, poDS);
        pTableDataset->SetEncoding(oEncoding);

        return wxStaticCast(pTableDataset, wxGISDataset);
    }
    return NULL;
}

/*
bool GeometryVerticesToTextFile(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxGISCoordinatesFormat &oFrmt, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
	CPLErrorReset();

    VSILFILE    *fpCoordsFile;

    fpCoordsFile = VSIFOpenL( sPath, "wt" );
    if( fpCoordsFile == NULL )
	{
		wxString sErr = wxString::Format(_("Error create text file! Path '%s'. OGR error: "), wxString(sPath, wxConvUTF8).c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
	}


    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
		pProgressor = pTrackCancel->GetProgressor();
    if(pProgressor)
        pProgressor->SetRange(pDSet->GetFeatureCount());

	int nCounter(0);
	long nFidCounter(0);
 	OGRFeatureSPtr poFeature;
	pDSet->Reset();
    while((poFeature = pDSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.mb_str());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
            return false;
        }

		nCounter++;
        if(pProgressor)
            pProgressor->SetValue(nCounter);


        OGRGeometry* pGeom = poFeature->GetGeometryRef();
        if(!pGeom)
            continue;

		CPLString osCoordText = GeometryToText(poFeature->GetFID(), pGeom, oFrmt, pTrackCancel);

		if(osCoordText.size() > 0)
		{
			VSIFWriteL( (void *) osCoordText.c_str(), 1, osCoordText.size(), fpCoordsFile );
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString::Format(_("Geometry No. %d added"), nCounter), wxNOT_FOUND, enumGISMessageInfo);
		}
		else
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(wxString(_("Unsupported geometry type")), wxNOT_FOUND, enumGISMessageWarning);
		}
	}

    VSIFCloseL( fpCoordsFile );

	return true;
}

CPLString GeometryToText(long nGeomFID, OGRGeometry* pGeom, wxGISCoordinatesFormat &oFrmt, ITrackCancel* pTrackCancel)
{
	CPLString osOutput;
	if(!pGeom)
		return osOutput;

	CPLString osTmp;
	osOutput += CPLSPrintf("Geometry %d (%s)\n", (int)nGeomFID, pGeom->getGeometryName());//CPLString().Printf
	OGRwkbGeometryType type = wkbFlatten(pGeom->getGeometryType());
	switch(type)
	{
	case wkbPoint:
		PointToText(osOutput, (OGRPoint*)pGeom, oFrmt);
		break;
	case wkbLineString:
	case wkbLinearRing:
		LineToText(osOutput, (OGRLineString*)pGeom, oFrmt);
		break;
	case wkbPolygon:
		{
            int nRings(0);
            OGRPolygon* poPoly = (OGRPolygon*)pGeom;
            OGRLinearRing* poRing = poPoly->getExteriorRing();
            nRings = poPoly->getNumInteriorRings();
            if (poRing == NULL)
                osOutput += CPLString("empty");
            else
            {
				osOutput += CPLString("Exterior ring\n");
				LineToText(osOutput, (OGRLineString*)poRing, oFrmt);

                if (nRings)
                {
                    osOutput += CPLSPrintf("%d inner rings\n", nRings);
                    for(int ir = 0; ir < nRings; ++ir)
                    {
						osOutput += CPLSPrintf("%d inner ring\n", ir);
						OGRLinearRing* poRing = poPoly->getInteriorRing(ir);
						LineToText(osOutput, (OGRLineString*)poRing, oFrmt);
                    }
                }
            }
		}
		break;
	case wkbMultiPolygon:
		//break;
	case wkbMultiPoint:
		//break;
	case wkbMultiLineString:
		//break;
	case wkbGeometryCollection:
		{
            OGRGeometryCollection* poColl = (OGRGeometryCollection*)pGeom;
            osOutput += CPLSPrintf("%d geometries:\n", poColl->getNumGeometries() );
            for (int ig = 0; ig < poColl->getNumGeometries(); ++ig)
            {
                OGRGeometry * poChild = (OGRGeometry*)poColl->getGeometryRef(ig);
                GeometryToText(nGeomFID, poChild, oFrmt, pTrackCancel);
            }
		}
		break;
	default:
	case wkbUnknown:
	case wkbNone:
		break;
	}
	return osOutput;
}

void PointToText(CPLString &osData, OGRPoint* pPoint, wxGISCoordinatesFormat &oFrmt)
{
	wxString sFrmt = oFrmt.Format(pPoint->getX(), pPoint->getY());
	sFrmt += wxString(wxT("\n"));
	osData += sFrmt.mb_str();
}

void LineToText(CPLString &osData, OGRLineString* pLine, wxGISCoordinatesFormat &oFrmt)
{
	int nCount = pLine->getNumPoints();
	if(nCount == 0)
	{
		osData += CPLString("empty\n");
		return;
	}

    osData += CPLSPrintf("%d points\n", nCount );
	for(size_t i = 0; i < nCount; ++i)
	{
		OGRPoint Pt;
		pLine->getPoint(i, &Pt);
		PointToText(osData, &Pt, oFrmt);
	}
}
*/