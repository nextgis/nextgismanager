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
#pragma once

#include "wxgis/carto/carto.h"

/*
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/framework/application.h"
#include "wxgis/geometry/geometry.h"

class wxGISFeatureTransformThread : public wxThread
{
public:
	wxGISFeatureTransformThread(wxGISFeatureDatasetSPtr pwxGISFeatureDataset, OGRCoordinateTransformation *poCT, bool bTransform, OGRPolygon* pRgn1, OGRPolygon* pRgn2, wxCriticalSection* pCritSect, OGREnvelope* pFullEnv, wxGISGeometrySet* pOGRGeometrySet, size_t &nCounter, ITrackCancel* pTrackCancel = NULL);
    virtual void *Entry();
    virtual void OnExit();
protected:
    //virtual OGRGeometry* CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv, OGRCoordinateTransformation *poCT);
    virtual OGRGeometry* CheckRgnAndTransform1(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT);
    virtual OGRGeometry* Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv);
protected:
    OGRPolygon *m_pRgn1, *m_pRgn2;
    ITrackCancel *m_pTrackCancel;
    wxGISFeatureDatasetSPtr m_pwxGISFeatureDataset;
    OGRCoordinateTransformation *m_poCT;
    bool m_bTransform;
    wxCriticalSection *m_pCritSect;
    OGREnvelope *m_pFullEnv;
    wxGISGeometrySet* m_pOGRGeometrySet;
    //progress
    size_t &m_nCounter;
    IProgressor *m_pProgressor;
    double m_fSegSize;
};
*/
