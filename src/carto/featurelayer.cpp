/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureLayer header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013,2014 Bishop
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
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/mxevent.h"

#define STEP 3.0
#define NOCACHEVAL 2000


//----------------------------------------------------------------------------
// wxGISFeatureLayer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISFeatureLayer, wxGISLayer)

BEGIN_EVENT_TABLE(wxGISFeatureLayer, wxGISLayer)
    EVT_DS_CLOSED(wxGISFeatureLayer::OnDSClosed)
	EVT_DS_FEATURES_ADDED(wxGISFeatureLayer::OnDSFeaturesAdded)
	EVT_DS_FEATURE_ADDED(wxGISFeatureLayer::OnDSFeatureAdded)
	EVT_DS_FEATURE_DELETED(wxGISFeatureLayer::OnDSFeatureDeleted)
	EVT_DS_FEATURE_CHANGED(wxGISFeatureLayer::OnDSFeatureChanged)
END_EVENT_TABLE()

wxGISFeatureLayer::wxGISFeatureLayer(const wxString &sName, wxGISDataset* pwxGISDataset) : wxGISLayer(sName, pwxGISDataset)
{
    m_pwxGISFeatureDataset = wxDynamicCast(pwxGISDataset, wxGISFeatureDataset);
	if(m_pwxGISFeatureDataset)
	{
        m_pwxGISFeatureDataset->Reference();
        if(m_sName.IsEmpty())
            m_sName = m_pwxGISFeatureDataset->GetName();

        m_SpatialReference = m_pwxGISFeatureDataset->GetSpatialReference();
		m_FullEnvelope = m_pwxGISFeatureDataset->GetEnvelope();

        //create new renderer
        m_pFeatureRenderer = new wxGISFeatureRenderer(this);
		m_pRenderer = wxStaticCast(m_pFeatureRenderer, wxGISRenderer);

        m_nConnectionPointDSCookie = m_pwxGISFeatureDataset->Advise(this);
	}
    m_pSpatialTree = NULL;
}

wxGISFeatureLayer::~wxGISFeatureLayer(void)
{
    if(m_nConnectionPointDSCookie != wxNOT_FOUND)
        m_pwxGISFeatureDataset->Unadvise(m_nConnectionPointDSCookie);
    wxDELETE(m_pSpatialTree);    
    wsDELETE(m_pwxGISFeatureDataset);
}

bool wxGISFeatureLayer::Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(m_pRenderer, false, wxT("The current renderer point is NULL"));
    return m_pRenderer->Draw(DrawPhase, m_pDisplay, pTrackCancel);

/*	if(m_pFeatureRenderer->CanRender(m_pwxGISFeatureDataset))
	{
	    bool bSetFilter(false);
		//Check if get all features
		OGREnvelope Env = pDisplay->GetBounds();
		if(!IsDoubleEquil(m_PreviousEnvelope.MaxX, Env.MaxX) || !IsDoubleEquil(m_PreviousEnvelope.MaxY, Env.MaxY) || !IsDoubleEquil(m_PreviousEnvelope.MinX, Env.MinX) || !IsDoubleEquil(m_PreviousEnvelope.MinY, Env.MinY))
		{
			OGREnvelope TempFullEnv = m_FullEnvelope;
			//use angle
			if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
			{
				double dCenterX = Env.MinX + (Env.MaxX - Env.MinX) / 2;
				double dCenterY = Env.MinY + (Env.MaxY - Env.MinY) / 2;

				RotateEnvelope(TempFullEnv, pDisplay->GetRotate(), dCenterX, dCenterY);
			}
			bSetFilter = TempFullEnv.Contains(Env) != 0;
		}

		//store envelope
		m_PreviousEnvelope = Env;

	    //Get features set
		wxGISQuadTreeCursorSPtr pCursor;
	    if(bSetFilter)
	    {
			const CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
			pCursor = m_pwxGISFeatureDataset->SearchGeometry(&Rect);
		}
		else
		{
			pCursor = m_pwxGISFeatureDataset->SearchGeometry();
		}
		m_pFeatureRenderer->Draw(pCursor, DrawPhase, pDisplay, pTrackCancel);
	}

	return true;*/
}

bool wxGISFeatureLayer::IsValid(void) const
{
	return m_pRenderer && m_pwxGISDataset && m_pwxGISDataset->IsOpened();
}

bool wxGISFeatureLayer::IsCacheNeeded(void) const
{
    //TODO: if added empty layer with adding a lot of features
    return true;

 	if(m_pwxGISFeatureDataset)
	{
        if(m_pwxGISFeatureDataset->GetFeatureCount() > NOCACHEVAL)
            return true;

	    //count number of features or points and if count < NOCACHEVAL set NoCache
		long nPointCount(0);
		wxGISFeature Feature;

		m_pwxGISFeatureDataset->Reset();
		while((Feature = m_pwxGISFeatureDataset->Next()).IsOk())
		{
            nPointCount += GetPointsInGeometry(Feature.GetGeometry());
			if(nPointCount > NOCACHEVAL)
				return true;
		}
	}
	return false;
}

OGRwkbGeometryType wxGISFeatureLayer::GetGeometryType(void) const
{
    if(m_pwxGISFeatureDataset)
        return m_pwxGISFeatureDataset->GetGeometryType();
    return wkbUnknown;
}

wxGISFeature wxGISFeatureLayer::GetFeatureByID(long nFID)
{
    if(m_pwxGISFeatureDataset)
        return m_pwxGISFeatureDataset->GetFeatureByID(nFID);
    return wxGISFeature();
}

long wxGISFeatureLayer::GetPointsInGeometry(const wxGISGeometry& Geom) const
{
    if(!Geom.IsOk())
        return 0;

	OGRwkbGeometryType type = wkbFlatten(Geom.GetType());
	switch(type)
	{
	case wkbPoint:
		return 1;
	case wkbPolygon:
		{
			OGRPolygon* pPoly = (OGRPolygon*)(OGRGeometry*)Geom;
			OGRLinearRing* pRing = pPoly->getExteriorRing();
			long nPointCount(0);
			if(pPoly)
			{
				nPointCount = GetPointsInGeometry(wxGISGeometry(pRing, false));
				for(int i = 0; i < pPoly->getNumInteriorRings(); ++i)
				{
					pRing = pPoly->getInteriorRing(i);
					nPointCount += GetPointsInGeometry(wxGISGeometry(pRing, false));
				}
			}
			return nPointCount;
		}
	case wkbLineString:
		{
			long nPointCount(0);
			OGRLineString* pLineString = (OGRLineString*)(OGRGeometry*)Geom;
			if(pLineString)
				nPointCount += pLineString->getNumPoints();
			return nPointCount;
		}
	case wkbLinearRing:
		{
			long nPointCount(0);
			OGRLinearRing* pLineString = (OGRLinearRing*)(OGRGeometry*)Geom;
			if(pLineString)
				nPointCount += pLineString->getNumPoints();
			return nPointCount;
		}
	case wkbMultiPoint:
	case wkbMultiPolygon:
	case wkbMultiLineString:
	case wkbGeometryCollection:
		{
			long nPointCount(0);
			OGRGeometryCollection* pGeometryCollection = (OGRGeometryCollection*)(OGRGeometry*)Geom;
			for(int i = 0; i < pGeometryCollection->getNumGeometries(); ++i)
				nPointCount += GetPointsInGeometry(wxGISGeometry(pGeometryCollection->getGeometryRef(i), false));
			return nPointCount;
		}
	case wkbUnknown:
	case wkbNone:
	default:
		return 0;
	}
}

wxGISSpatialTreeCursor wxGISFeatureLayer::SearchGeometry(const OGREnvelope &Env)
{
    wxGISSpatialTreeCursor Cursor;
	if(m_pSpatialTree)
	{
		Cursor = m_pSpatialTree->Search(Env);
	}
	else
	{
		Cursor = m_pwxGISFeatureDataset->SearchGeometry(Env);
	}
    return Cursor;
}

wxGISSpatialTreeCursor wxGISFeatureLayer::Idetify(const wxGISGeometry &Geom)
{
    OGREnvelope Env = Geom.GetEnvelope();

	wxGISSpatialTreeCursor Cursor = SearchGeometry(Env);
    wxGISSpatialTreeCursor retCursor;
	//intersect geoms & set to NULL
    wxGISSpatialTreeCursor::const_iterator iter;
    for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
    {    
        wxGISSpatialTreeData *pItem = *iter;

        if(pItem && Geom.Intersects(pItem->GetGeometry()))
        {
            retCursor.push_back(pItem);
        }
    }

	return retCursor;
}

void wxGISFeatureLayer::OnDSClosed(wxFeatureDSEvent& event)
{
    wxDELETE(m_pSpatialTree);
    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_DS_CLOSED, GetId());
    AddEvent(wxMxMapViewEvent_);
}

void wxGISFeatureLayer::OnDSFeaturesAdded(wxFeatureDSEvent& event)
{
    wxCHECK_RET(m_pFeatureRenderer, wxT("The current renderer point is NULL"));
    //if reprojected - add to quadtree and draw from it
    if(m_pSpatialTree)
    {
        wxGISSpatialTreeCursor Cursor = event.GetCursor();
        wxGISSpatialTreeCursor retCursor; 

        wxGISSpatialTreeCursor::const_iterator iter;
        for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
        {    
            wxGISSpatialTreeData *current = *iter;
            wxGISGeometry Geom(current->GetGeometry().Copy());
            if(Geom.Project(m_SpatialReference))
            {
                long nOid = current->GetFID();
                if(!m_pSpatialTree->HasFID(nOid))
                {
                    wxGISSpatialTreeData *pNewItem = new wxGISSpatialTreeData(Geom, nOid);
                    retCursor.push_back(pNewItem);

                    m_pSpatialTree->Insert(pNewItem);
                }
            }
        }

        m_pFeatureRenderer->Draw(retCursor, wxGISDPGeography, m_pDisplay);
    }
    else
    {
        m_pFeatureRenderer->Draw(event.GetCursor(), wxGISDPGeography, m_pDisplay);
    }
    //send event that layer is changed and redraw needed to upper layers and whole map
    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_LOADING, GetId());
    AddEvent(wxMxMapViewEvent_);
}

void wxGISFeatureLayer::OnDSFeatureAdded(wxFeatureDSEvent& event)
{
    wxGISFeature Feature = m_pwxGISFeatureDataset->GetFeatureByID(event.GetFID());
    if(m_pSpatialTree)
    {
        wxGISGeometry Geom(Feature.GetGeometry().Copy());
        bool bProj = Geom.Project(m_SpatialReference);

        if(!m_pSpatialTree->HasFID(Feature.GetFID()))
        {
            m_pSpatialTree->Insert(Geom, Feature.GetFID());
        }

        if(bProj)
        {
            if(m_FullEnvelope.IsInit())
                m_FullEnvelope.Merge(Geom.GetEnvelope());
            else
                m_FullEnvelope = Geom.GetEnvelope();
        }
    }
    else
    {
        if(m_FullEnvelope.IsInit())
            m_FullEnvelope.Merge(Feature.GetGeometry().GetEnvelope());
        else
            m_FullEnvelope = Feature.GetGeometry().GetEnvelope();
    }

    //notify renderer
    if (m_pFeatureRenderer)
    {
        m_pFeatureRenderer->FeatureChanged(Feature);
    }

    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_CHANGED, GetId());
    AddEvent(wxMxMapViewEvent_);
}

void wxGISFeatureLayer::OnDSFeatureDeleted(wxFeatureDSEvent& event)
{
    if(m_pSpatialTree)
    {
        m_pSpatialTree->Remove(event.GetFID());
    }

    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_CHANGED, GetId());
    AddEvent(wxMxMapViewEvent_);
}

void wxGISFeatureLayer::OnDSFeatureChanged(wxFeatureDSEvent& event)
{
    wxLogDebug(wxT("changed: %d"), event.GetFID());
    wxGISFeature Feature = m_pwxGISFeatureDataset->GetFeatureByID(event.GetFID());
    if(m_pSpatialTree)
    {
        wxGISGeometry Geom(Feature.GetGeometry().Copy());
        bool bProj = Geom.Project(m_SpatialReference);

        m_pSpatialTree->Change(Geom, Feature.GetFID());

        if(bProj)
        {
            if(m_FullEnvelope.IsInit())
                m_FullEnvelope.Merge(Geom.GetEnvelope());
            else
                m_FullEnvelope = Geom.GetEnvelope();
        }
    }
    else
    {
        if(m_FullEnvelope.IsInit())
            m_FullEnvelope.Merge(Feature.GetGeometry().GetEnvelope());
        else
            m_FullEnvelope = Feature.GetGeometry().GetEnvelope();
    }

    //notify renderer
    if (m_pFeatureRenderer)
    {
        m_pFeatureRenderer->FeatureChanged(Feature);
    }

    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_CHANGED, GetId());
    AddEvent(wxMxMapViewEvent_);
}

void wxGISFeatureLayer::SetSpatialReference(const wxGISSpatialReference &SpatialReference)
{
    m_FullEnvelope = m_pwxGISFeatureDataset->GetEnvelope();
    //delete previous quadtree
    if(m_pSpatialTree)
    {
        wxDELETE(m_pSpatialTree);
    }    
    if(m_SpatialReference->IsSame(SpatialReference))
        return;

    m_SpatialReference = SpatialReference;
    wxGISGeometry newGeom = EnvelopeToGeometry(m_FullEnvelope, m_pwxGISFeatureDataset->GetSpatialReference());
    if(newGeom.IsOk() && newGeom.Project(SpatialReference))
    {
        m_FullEnvelope = newGeom.GetEnvelope();
    }

	//project and load to quadtree
    m_pSpatialTree = CreateSpatialTree(m_pwxGISFeatureDataset);
    m_pSpatialTree->Load(SpatialReference);
}

void wxGISFeatureLayer::SetRenderer(wxGISRenderer* pRenderer)
{
    wxGISLayer::SetRenderer(pRenderer);
    m_pFeatureRenderer = wxStaticCast(pRenderer, wxGISFeatureRenderer);
}

bool wxGISFeatureLayer::IsLoading() const
{
    if (NULL != m_pSpatialTree)
    {
        return m_pSpatialTree->IsLoading();
    }
    
    return false;
}

/*
void wxGISFeatureLayer::LoadGeometry(void)
{
  if(!IsValid() && m_bIsGeometryLoaded)
        return;
    UnloadGeometry();
    m_FullEnv.MinX = m_FullEnv.MaxX = m_FullEnv.MinY = m_FullEnv.MaxY = 0;

    if(!m_pwxGISFeatureDataset->Open())
    {
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISFeatureLayer: Open failed! GDAL error: %s"), wgMB2WX(err));
		wxLogError(sErr);
		wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
        return;
    }

    bool bTransform(false);
    OGRPolygon* pRgn1 = NULL;
    OGRPolygon* pRgn2 = NULL;
    OGRCoordinateTransformation *poCT = NULL;

	const OGRSpatialReferenceSPtr pDatasetSpaRef = m_pwxGISFeatureDataset->GetSpatialReference();
    //get cut poly
    if( pDatasetSpaRef )
    {
	    if(m_pSpatialReference && pDatasetSpaRef)
        {
            if(!m_pSpatialReference->IsSame(pDatasetSpaRef.get()))
            {
                //get envelope
                wxGISSpatialReferenceValidator GISSpaRefValidator;
                wxString sProjName;
                if(m_pSpatialReference->IsProjected())
                    sProjName = wgMB2WX(m_pSpatialReference->GetAttrValue("PROJCS"));
                else
                    sProjName = wgMB2WX(m_pSpatialReference->GetAttrValue("GEOGCS"));
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
                        pRgn1->segmentize(STEP);
                    }
                    if(pRgn2 != NULL)
                    {
                        pRgn2->assignSpatialReference(pWGSSpaRef);
                        pRgn2->segmentize(STEP);
                    }
                    pWGSSpaRef->Dereference();

                    if(!pDatasetSpaRef->IsSame(pWGSSpaRef))
                    {
                        if(pRgn1->transformTo(pDatasetSpaRef.get()) != OGRERR_NONE)
                            wxDELETE(pRgn1);
                        if(pRgn2->transformTo(pDatasetSpaRef.get()) != OGRERR_NONE)
                            wxDELETE(pRgn2);
                    }
                }
                //create cut poly
                bTransform = true;
            }
        }

        poCT = OGRCreateCoordinateTransformation( pDatasetSpaRef.get(), m_pSpatialReference );
    }

    IApplication* pApp = ::GetApplication();
    IStatusBar* pStatusBar = pApp->GetStatusBar();
    IProgressor* pProgressor = pStatusBar->GetProgressor();
    if(pProgressor)
    {
        pProgressor->Show(true);
        pProgressor->SetRange(m_pwxGISFeatureDataset->GetFeatureCount() * 2);
    }

    //strart multithreaded transform
    int CPUCount = wxThread::GetCPUCount();//1;//
    std::vector<wxGISFeatureTransformThread*> threadarray;
    wxCriticalSection CritSect;
    size_t nCounter(0);

#ifdef __WXDEBUG__
    wxStopWatch sw;
#endif

    //OGRFeature* pF;
    //long counter = 0;
    //while((pF = m_pwxGISFeatureDataset->Next()) != NULL)
    //{
    //    counter++;
    //    OGRFeature::DestroyFeature(pF);
    //}

    ITrackCancel TrackCancel;
    TrackCancel.SetProgressor(pProgressor);

    m_pwxGISFeatureDataset->Reset();
    for(int i = 0; i < CPUCount; ++i)
    {
        wxGISFeatureTransformThread *thread = new wxGISFeatureTransformThread(m_pwxGISFeatureDataset, poCT, bTransform, pRgn1, pRgn2, &CritSect, &m_FullEnv, m_pOGRGeometrySet, nCounter, &TrackCancel);
        thread->Create();
        thread->Run();
        threadarray.push_back(thread);
    }

    for(size_t i = 0; i < threadarray.size(); ++i)
    {
		if(threadarray[i]->IsAlive())
			wgDELETE(threadarray[i], Wait())
		else
			wxDELETE(threadarray[i]);
    }

#ifdef __WXDEBUG__
    wxLogDebug(wxT("The long running function took %ldms to execute"), sw.Time());
#endif

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);
    wxDELETE(pRgn1);
    wxDELETE(pRgn2);

    if(IsDoubleEquil(m_FullEnv.MinX, m_FullEnv.MaxX))
    {
        m_FullEnv.MaxX += 1;
        m_FullEnv.MinX -= 1;
    }
    if(IsDoubleEquil(m_FullEnv.MinY, m_FullEnv.MaxY))
    {
        m_FullEnv.MaxY += 1;
        m_FullEnv.MinY -= 1;
    }

    CreateQuadTree(&m_FullEnv);

	size_t nStep = m_pwxGISFeatureDataset->GetFeatureCount() < 10 ? 1 : m_pwxGISFeatureDataset->GetFeatureCount() / 10;

    m_pOGRGeometrySet->Reset();
    OGRGeometry* pOutGeom;

    CPLRectObj Rect = {m_FullEnv.MinX, m_FullEnv.MinY, m_FullEnv.MaxX, m_FullEnv.MaxY};
    CPLQuadTree* pQuadTree1(NULL),* pQuadTree2(NULL),* pQuadTree3(NULL),* pQuadTree4(NULL),* pQuadTree5(NULL),* pQuadTree6(NULL);


    while((pOutGeom = m_pOGRGeometrySet->Next()) != NULL)
    {
	    if(m_pQuadTree)
		    CPLQuadTreeInsert(m_pQuadTree, pOutGeom);

        nCounter++;
        if(pProgressor && nCounter % nStep == 0)
            pProgressor->SetValue(nCounter);

    }

    if(pProgressor)
        pProgressor->Show(false);
    pStatusBar->SetMessage(_("Done"));
    m_bIsGeometryLoaded = true;

}
*/
