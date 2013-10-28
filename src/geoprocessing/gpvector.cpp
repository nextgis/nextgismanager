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
#include "wxgis/datasource/table.h"
#include "wxgis/datasource/featuredataset.h"
//#include "wxgis/geometry/algorithm.h"
//#include "wxgis/datasource/spvalidator.h"

/*
#define ENCODE_STRINGS //encode string while copy records

bool CopyRows(wxGISFeatureDatasetSPtr pSrcDataSet, wxGISFeatureDatasetSPtr pDstDataSet, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    const OGRSpatialReferenceSPtr pSrsSRS = pSrcDataSet->GetSpatialReference();
    const OGRSpatialReferenceSPtr pDstSRS = pDstDataSet->GetSpatialReference();
    OGRCoordinateTransformation *poCT(NULL);
    bool bSame = pSrsSRS == NULL || pDstSRS == NULL || pSrsSRS->IsSame(pDstSRS.get());
    if( !bSame )
        poCT = OGRCreateCoordinateTransformation( pSrsSRS.get(), pDstSRS.get() );

    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString::Format(_("Start CopyRows from '%s' to '%s'"), wxString(pSrcDataSet->GetPath(), wxConvUTF8).c_str(), wxString(pDstDataSet->GetPath(), wxConvUTF8).c_str()), wxNOT_FOUND, enumGISMessageNorm);
    }
    int nCounter(0);
    if(pProgressor)
        pProgressor->SetRange(pSrcDataSet->GetFeatureCount());

    pSrcDataSet->Reset();
    OGRFeatureSPtr pFeature;
    while((pFeature = pSrcDataSet->Next()) != NULL)
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
        OGRGeometry *pGeom = pFeature->GetGeometryRef();
		if(wkbFlatten(pDstDataSet->GetGeometryType()) != wkbUnknown && !pGeom)
            continue;

        if( !bSame && poCT )
        {
            if(pGeom)
            {
				OGRGeometry *pNewGeom = pGeom->clone();
                OGRErr eErr = pNewGeom->transform(poCT);
                if(eErr == OGRERR_NONE)
                    pFeature->SetGeometryDirectly(pNewGeom);
                else
                    wxDELETE(pNewGeom);
            }
        }
        //set encoding
#ifdef ENCODE_STRINGS
        //if(1)//OutputEncoding != InputEncoding)
        //{
        //wxFontEncoding InputEncoding = pSrcDataSet->GetEncoding();
        wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();

        OGRFeatureDefn *pFeatureDefn = pFeature->GetDefnRef();
        if(!pFeatureDefn)
            continue;

        CPLErrorReset();
        wxString sACText;

        for(size_t i = 0; i < pFeatureDefn->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
            if(pFieldDefn)
            {
                OGRFieldType nType = pFieldDefn->GetType();
                if(OFTString == nType)
                {
                    wxString sFieldString;
                    if(CPLIsUTF8(pFeature->GetFieldAsString(i), -1))
                    {
                        sFieldString = wxString::FromUTF8(pFeature->GetFieldAsString(i));
                    }
                    else
                    {
                        sFieldString = wxString(pFeature->GetFieldAsString(i), wxConvLocal);
                    }
                    //wxCSConv inconv(OutputEncoding);
                    //CPLString cplstr(pFeature->GetFieldAsString(i));
                    //wxString sFieldString(cplstr.
                    ////wxString sFieldString(wgMB2WX(pFeature->GetFieldAsString(i)));
                    //wxCSConv outconv(OutputEncoding);
                    //pFeature->SetField(i, sFieldString.mb_str(outconv));

                    if(OutputEncoding <= wxFONTENCODING_DEFAULT)
						pFeature->SetField(i, sFieldString.mb_str());
                    else
                    {
                        wxCSConv outconv(OutputEncoding);
                        pFeature->SetField(i, sFieldString.mb_str(outconv));
                    }
                    if(pDstDataSet->GetSubType() == enumVecDXF)
                        sACText += sFieldString + wxT("\n");
                }
                else if(OFTTime == nType && pDstDataSet->GetSubType() == enumVecESRIShapefile)
                {
                    wxString sFieldString(pFeature->GetFieldAsString(i), wxConvLocal);
    				pFeature->SetField(i, sFieldString.mb_str());
                }
                //TODO: OFTStringList
            }
        }

        if(pDstDataSet->GetSubType() == enumVecDXF)
        {
        //    //LABEL(f:"Arial, Helvetica", s:12pt, t:"Hello World!")
            wxString sStyleLabel = wxString::Format(wxT("LABEL(f:\"Arial, Helvetica\", s:12pt, t:\"%s\")"), sACText.c_str());
			pFeature->SetStyleString(sStyleLabel.mb_str(wxConvLocal));
        }

        //}
#endif
        OGRErr eErr = pDstDataSet->StoreFeature(pFeature);
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

    return true;
}

bool ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();

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

    //copy data
    if(!CopyRows(pDSet, pNewDSet, pQFilter, pTrackCancel))
    {
        wxString sErr(_("Error copying data to a new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return false;
    }
    return true;
}

bool ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Exporting %s to %s"), pDSet->GetName().c_str(), sName.c_str()), wxNOT_FOUND, enumGISMessageTitle);
    const OGRSpatialReferenceSPtr pSrcSpaRef = pDSet->GetSpatialReference();
    if(!pSrcSpaRef && (nNewSubType == enumVecKML || nNewSubType == enumVecKMZ))
    {
        wxString sErr(_("Input spatial reference is not defined!"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    OGRSpatialReference* pNewSpaRef(NULL);
    if(nNewSubType == enumVecKML || nNewSubType == enumVecKMZ)
        pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
    else if(pSrcSpaRef)
        pNewSpaRef = pSrcSpaRef->Clone();

    OGRFeatureDefn *pDef = pDSet->GetDefinition();
    if(!pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

     //check multi geometry
    OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
	size_t nFullCount = pDSet->GetFeatureCount();
    bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
	bool bExported(false);

    if(bIsMultigeom)
    {
        wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
			size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_point")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbPoint );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
				bExported = true;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_polygon")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbPolygon );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
				bExported = true;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_line")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbLineString );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
				bExported = true;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mpoint")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiPoint );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
				bExported = true;
            }
       }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mline")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiLineString );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
				bExported = true;
            }
       }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            size_t nCount = pDSet->GetFeatureCount();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mpolygon")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiPolygon );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
	 			bExported = true;
            }
		}

		pDSet->SetFilter(NULL);
		if(!bExported && nFullCount > 0) //assume we have only one type geometry
		{
			//set geometry type
			pDSet->Reset();
			OGRFeatureSPtr pFeature;
			while((pFeature = pDSet->Next()) != NULL)
			{
				OGRGeometry* pG = pFeature->GetGeometryRef();
				if(pG)
				{
					OGRwkbGeometryType eGType = pG->getGeometryType();
					pDef->SetGeomType(eGType);
					break;
				}
			}
			if( !ExportFormat(pDSet, sPath, sName, pFilter, pDef, pNewSpaRef, pQFilter, pTrackCancel) )
				return false;
		}
    }
    else
    {
        if( !ExportFormat(pDSet, sPath, sName, pFilter, pDef, pNewSpaRef, pQFilter, pTrackCancel) )
            return false;
    }
    return true;
}

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
wxGISDataset* CreateDataset(const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, OGRFeatureDefn* const poFields, wxGISSpatialReference oSpatialRef, OGRwkbGeometryType eGType, char ** papszDataSourceOptions, char ** papszLayerOptions, ITrackCancel* const pTrackCancel)
{

    bool bIsFilerValid = pFilter && (pFilter->IsKindOf(wxCLASSINFO(wxGxFeatureDatasetFilter)) || pFilter->IsKindOf(wxCLASSINFO(wxGxTableDatasetFilter)));
    if(!bIsFilerValid)
    {
        CPLError( CE_Failure, CPLE_FileIO, "Input data is invalid: unsuported filter" );
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Input data is invalid: unsuported filter"), wxNOT_FOUND, enumGISMessageErr);
        return NULL;
    }

    CPLErrorReset();

    //poFields->Reference();
	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pFilter->GetDriver().mb_str() );
    if(poDriver == NULL)
    {
        wxString sErr = wxString::Format(_("The driver '%s' is not available! OGR error: "), pFilter->GetDriver().c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        return NULL;
    }

    OGRDataSource *poDS = poDriver->CreateDataSource(sPath, papszDataSourceOptions );
    if(poDS == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output file '%s'! OGR error: "), wxString(sPath, wxConvUTF8).c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
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
    sNewName.Truncate(27);

	if(wxIsdigit(sNewName.GetChar(0)))
		sNewName.Prepend(_("Layer_"));

    CPLString szName;
    if(pFilter->GetType() == enumGISFeatureDataset)
    {
        if(pFilter->GetSubType() == enumVecKMZ)
		    szName = Transliterate(CPLString(sNewName.mb_str()).c_str());
	    else if(pFilter->GetSubType() == enumVecKML)
		    szName = CPLString(sNewName.mb_str(wxConvUTF8));//wxCSConv(wxT("cp-866"))));
        else
            szName = CPLString(sNewName.mb_str());
    }
    else
    {
        szName = CPLString(sNewName.mb_str());
    }

    //set default encoding for layer
    if(CSLFindName(papszLayerOptions, "ENCODING") == -1)
        papszLayerOptions = CSLAddString( papszLayerOptions, "ENCODING=UTF-8" );

    CPLSetConfigOption("SHAPE_ENCODING", "");
	OGRLayer *poLayerDest = poDS->CreateLayer(szName, oSpatialRef, eGType, papszLayerOptions );
    if(poLayerDest == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
        CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);
        return NULL;
    }

	CPLString szNameField, szDescField;
    if(pFilter->GetType() == enumGISFeatureDataset)
    {
        if(pFilter->GetSubType() != enumVecDXF)
        {
            for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
            {
                OGRFieldDefn *pField = poFields->GetFieldDefn(i);
                OGRFieldDefn oFieldDefn( pField );
                if(pFilter->GetSubType() == enumVecKML || pFilter->GetSubType() == enumVecKMZ)
                {
                    wxString sFieldName(pField->GetNameRef(), wxConvLocal);
                    oFieldDefn.SetName(sFieldName.mb_str(wxConvUTF8));
				    OGRFieldType nType = pField->GetType();
				    if(OFTString == nType)
				    {
					    if(szNameField.empty())
						    szNameField = pField->GetNameRef();
					    if(szDescField.empty())
						    szDescField = pField->GetNameRef();
				    }
                }

                if(pFilter->GetSubType() == enumVecESRIShapefile && pField->GetType() == OFTTime)
                {
                    oFieldDefn.SetType(OFTString);
                    wxString sErr(_("Unsupported type for shape file - OFTTime. Change to OFTString."));
                    wxLogWarning(sErr);
                    CPLError( CE_Warning, CPLE_AppDefined, CPLString(sErr.mb_str()) );
                    if(pTrackCancel)
                        pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageWarning);
                }

	            if( poLayerDest->CreateField( &oFieldDefn ) != OGRERR_NONE )
	            {
                    wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
                    CPLString sFullErr(sErr.mb_str());
                    sFullErr += CPLGetLastErrorMsg();
                    CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
                    if(pTrackCancel)
                        pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
                    CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);
                    return NULL;
                }
            }
            if(pFilter->GetSubType() == enumVecKML || pFilter->GetSubType() == enumVecKMZ)
	        {
		        if(!szNameField.empty())
			        CPLSetConfigOption( "LIBKML_NAME_FIELD", szNameField );
		        if(!szDescField.empty())
			        CPLSetConfigOption( "LIBKML_DESCRIPTION_FIELD", szDescField );
		        //CPLSetConfigOption( "LIBKML_TIMESTAMP_FIELD", "YES" );
	        }
	        poFields->Release();
    
            CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);

            return wxStaticCast(new wxGISFeatureDataset(sPath, pFilter->GetSubType(), poLayerDest, poDS), wxGISDataset);
        }
        else
        {
            for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
            {
                OGRFieldDefn *pField = poFields->GetFieldDefn(i);
                if(pFilter->GetSubType() == enumVecKML || pFilter->GetSubType() == enumVecKMZ)
                {
                    wxString sFieldName(pField->GetNameRef(), wxConvLocal);
                    pField->SetName(sFieldName.mb_str(wxConvUTF8));
				    OGRFieldType nType = pField->GetType();
				    if(OFTString == nType)
				    {
					    if(szNameField.empty())
						    szNameField = pField->GetNameRef();
					    if(szDescField.empty())
						    szDescField = pField->GetNameRef();
				    }
                }

                if(pFilter->GetSubType() == enumTableDBF && pField->GetType() == OFTTime)
                {
                    pField->SetType(OFTString);
                    wxString sErr(_("Unsupported type for shape file - OFTTime. Change to OFTString."));
                    wxLogWarning(sErr);
                    CPLError( CE_Warning, CPLE_AppDefined, CPLString(sErr.mb_str()) );
                    if(pTrackCancel)
                        pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageWarning);
                }

	            if( poLayerDest->CreateField( pField ) != OGRERR_NONE )
	            {
                    wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sNewName.c_str());
                    CPLString sFullErr(sErr.mb_str());
                    sFullErr += CPLGetLastErrorMsg();
                    CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
                    if(pTrackCancel)
                        pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), wxNOT_FOUND, enumGISMessageErr);
                    CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);
                    return NULL;
                }
            }
        }
	    poFields->Release();
    
        CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);

        return wxStaticCast(new wxGISTable(sPath, pFilter->GetSubType(), poLayerDest, poDS), wxGISDataset);
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