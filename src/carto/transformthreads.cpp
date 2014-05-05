/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Transformation Threads header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
//#define FAST_BISHOP_INTERSECTION

#include "wxgis/carto/transformthreads.h"
/*
#include "wxgis/geometry/algorithm.h"



wxGISFeatureTransformThread::wxGISFeatureTransformThread(wxGISFeatureDatasetSPtr pwxGISFeatureDataset, OGRCoordinateTransformation *poCT, bool bTransform, OGRPolygon* pRgn1, OGRPolygon* pRgn2, wxCriticalSection* pCritSect, OGREnvelope* pFullEnv, wxGISGeometrySet* pOGRGeometrySet, size_t &nCounter, ITrackCancel* pTrackCancel) : wxThread(wxTHREAD_JOINABLE), m_nCounter(nCounter)
{
    m_pwxGISFeatureDataset = pwxGISFeatureDataset;
    m_poCT = poCT;
    m_pTrackCancel = pTrackCancel;
    m_bTransform = bTransform;
    m_pRgn1 = pRgn1;
    m_pRgn2 = pRgn2;
    m_pCritSect = pCritSect;
    m_pFullEnv = pFullEnv;
    m_pOGRGeometrySet = pOGRGeometrySet;
    m_pProgressor = pTrackCancel->GetProgressor();
//??
    if(m_poCT && m_poCT->GetSourceCS()->IsGeographic())
        m_fSegSize = 3.0;
    else
        m_fSegSize = 130.0;
}

void *wxGISFeatureTransformThread::Entry()
{
	int nFeatureCount = m_pwxGISFeatureDataset->GetFeatureCount();
    
    size_t nStep = nFeatureCount < 10 ? 1 : nFeatureCount / 10;

    m_pFullEnv->Merge(*m_pwxGISFeatureDataset->GetEnvelope());

 	OGRFeatureSPtr poFeature;
    while((poFeature = m_pwxGISFeatureDataset->Next()) != NULL)	
    {
        if(m_pTrackCancel && !m_pTrackCancel->Continue())
            return NULL;

        OGRGeometry* pGeom = poFeature->StealGeometry();//GetGeometryRef();   
        if(!pGeom)
            continue;

        OGREnvelope GEnv;
        pGeom->getEnvelope(&GEnv);
        
        if(!m_pFullEnv->Contains(GEnv))
            continue;

		OGRGeometry* pFeatureGeom = pGeom;//->clone();
        long nOID = poFeature->GetFID();

        if(m_bTransform)
        {
            if(m_pRgn1 == NULL && m_pRgn2 == NULL)
            {
                if(pFeatureGeom->transform( m_poCT ) != OGRERR_NONE)
                {
                    wxDELETE(pFeatureGeom);
                    continue;
                }
            }
            else
            {

                OGREnvelope *pRgnEnv1(NULL), *pRgnEnv2(NULL);
                if(m_pRgn1)
                {
                    pRgnEnv1 = new OGREnvelope();
                    m_pRgn1->getEnvelope(pRgnEnv1);
                }
                if(m_pRgn2)
                {
                    pRgnEnv2 = new OGREnvelope();
                    m_pRgn2->getEnvelope(pRgnEnv2);
                }

                //speed fix needed fo diff types of geom
                OGRGeometry* pGeom = CheckRgnAndTransform1(pFeatureGeom, m_pRgn1, m_pRgn2, pRgnEnv1, pRgnEnv2, m_poCT);
                wxDELETE(pRgnEnv1);
                wxDELETE(pRgnEnv2);
                wxDELETE(pFeatureGeom);

                if(pGeom)
                    pFeatureGeom = pGeom;
                //OGRGeometry* pGeom1 = CheckRgnAndTransform(pFeatureGeom, m_pRgn1, pRgnEnv1, m_poCT);
                //OGRGeometry* pGeom2 = CheckRgnAndTransform(pFeatureGeom, m_pRgn2, pRgnEnv2, m_poCT);
                //OGRGeometry *pGeom(NULL), *pGeom1(NULL), *pGeom2(NULL);

                //if(m_pRgn1)
                //    pGeom1 = pFeatureGeom->Intersection(m_pRgn1);
                //if(m_pRgn2)
                //    pGeom2 = pFeatureGeom->Intersection(m_pRgn2);

                //if(pGeom1 && pGeom1->transform( m_poCT ) != OGRERR_NONE)
                //{
                //    wxDELETE(pGeom1);
                //}  
                //if(pGeom2 && pGeom2->transform( m_poCT ) != OGRERR_NONE)
                //{
                //    wxDELETE(pGeom2);
                //}  
                //if(pGeom1 && !pGeom2)
                //    poFeature->SetGeometryDirectly(pGeom1); 
                //else if(pGeom2 && !pGeom1)
                //    poFeature->SetGeometryDirectly(pGeom2); 
                //else if(pGeom1 && pGeom2)
                //{
                //    OGRGeometryCollection* pGeometryCollection = new OGRGeometryCollection();
                //    pGeometryCollection->addGeometryDirectly(pGeom1);
                //    pGeometryCollection->addGeometryDirectly(pGeom2);
                //    poFeature->SetGeometryDirectly(pGeometryCollection); 
                //}
                else
                    continue;
            }
        }

        //OGREnvelope Env;
        //pFeatureGeom->getEnvelope(&Env);
        m_pCritSect->Enter();

        //m_pFullEnv->Merge(Env);
	    m_pOGRGeometrySet->AddGeometry(pFeatureGeom, nOID);

        m_nCounter++;
        if(m_pProgressor && m_nCounter % nStep == 0)
            m_pProgressor->SetValue(m_nCounter);

        m_pCritSect->Leave();
    }
    
	return NULL;
}
void wxGISFeatureTransformThread::OnExit()
{
}

OGRGeometry* wxGISFeatureTransformThread::CheckRgnAndTransform1(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT)
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
            pGeom1= Intersection(pFeatureGeom, pRgn1, pRgnEnv1);
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

OGRGeometry* wxGISFeatureTransformThread::Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv)
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
    case wkbMultiLineString:
    case wkbMultiPolygon:
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
*/
