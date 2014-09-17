/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Various Spatial Tree classes
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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

#include "wxgis/datasource/spatialtree.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/vectorop.h"

#define PRELOAD_GEOM_COUNT 2000

wxGISSpatialTreeCursor wxNullSpatialTreeCursor;

#define USE_R_STAR_TREE
//#define USE_QUAD_TREE

wxGISSpatialTree* CreateSpatialTree(wxGISFeatureDataset *pDS)
{
    wxGISSpatialTree* pSpatialTree(NULL);
#if defined USE_R_STAR_TREE
    pSpatialTree = new wxGISRTree(pDS, 64, 32);
#else ifdefined USE_QUAD_TREE
    pSpatialTree = new wxGISQuadTree(pDS);
#endif

    return pSpatialTree;
}

//-----------------------------------------------------------------------------
// wxGISSpatialTreeData
//-----------------------------------------------------------------------------

wxGISSpatialTreeData::wxGISSpatialTreeData(const wxGISGeometry &Geom, long nFID)
{
    m_Geom = Geom;
    m_nFID = nFID;
}

wxGISSpatialTreeData::~wxGISSpatialTreeData()
{
}

void wxGISSpatialTreeData::SetFID(long nFID)
{
    m_nFID = nFID;
}

long wxGISSpatialTreeData::GetFID(void) const
{
    return m_nFID;
}

wxGISGeometry wxGISSpatialTreeData::GetGeometry(void) const
{
    return m_Geom;
}

void wxGISSpatialTreeData::SetGeometry(const wxGISGeometry &oGeom)
{
    m_Geom = oGeom;
}

wxGISSpatialTreeData* wxGISSpatialTreeData::Clone() const
{
    return new wxGISSpatialTreeData(m_Geom.Clone(), m_nFID);
}


//-----------------------------------------------------------------------------
// wxGISSpatialTree
//----------------------------------------------------------------------------

wxGISSpatialTree::wxGISSpatialTree(wxGISFeatureDataset* pDSet) : wxThreadHelper(wxTHREAD_DETACHED)
{
    wsSET(m_pDSet, pDSet);

    m_nPreloadItemCount = PRELOAD_GEOM_COUNT;
    m_nReadPos = 0;

    m_bIsLoaded = false;
}

wxGISSpatialTree::~wxGISSpatialTree(void)
{
    DestroyLoadGeometryThread();

    wsDELETE(m_pDSet);
}

bool wxGISSpatialTree::Load(const wxGISSpatialReference &SpatRef, ITrackCancel* const pTrackCancel)
{

	if (m_bIsLoaded)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);
    m_pTrackCancel = pTrackCancel;
    m_SpatialReference = SpatRef;

    //create quad tree
    if(m_pDSet != NULL)
    {
        if(IsLoading())
            return true;

	    return CreateAndRunLoadGeometryThread();
    }
    return false;
}

bool wxGISSpatialTree::IsLoading(void) const
{
    return GetThread() && GetThread()->IsRunning();
}

void wxGISSpatialTree::CancelLoading()
{
    DestroyLoadGeometryThread();
}

wxThread::ExitCode wxGISSpatialTree::Entry()
{
    long nFeaturesCount = m_pDSet->GetFeatureCount(TRUE, m_pTrackCancel);
    if(nFeaturesCount == 0)
    {
        m_pTrackCancel = NULL;
        return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
    }

    IProgressor* pProgress(NULL);
	if(m_pTrackCancel)
	{
		m_pTrackCancel->Reset();
		m_pTrackCancel->PutMessage(wxString(_("PreLoad Geometry of ")) + m_pDSet->GetName(), -1, enumGISMessageInfo);
        pProgress = m_pTrackCancel->GetProgressor();
	}

    long nItemCounter(0);


	if(pProgress)
    {
        pProgress->SetRange(nFeaturesCount);
        pProgress->ShowProgress(true);
    }

    m_pDSet->Reset();

    //get only geometries if no filter is set
    wxArrayString saIgnoredFields;
    if (!m_pDSet->HasFilter())
    {
        saIgnoredFields = m_pDSet->GetFieldNames();
        saIgnoredFields.Add(wxT("OGR_STYLE"));
        m_pDSet->SetIgnoredFields(saIgnoredFields);
    }

    //reprojecion
    OGRCoordinateTransformation *poCT = NULL;
    if(m_SpatialReference.IsOk() && !m_SpatialReference.IsSame(m_pDSet->GetSpatialReference()))
    {
        poCT = OGRCreateCoordinateTransformation( m_pDSet->GetSpatialReference(), m_SpatialReference);
    }

    wxVector<wxGISSpatialTreeData*> m_paCachedData;
    m_paCachedData.reserve(m_nPreloadItemCount);

    wxGISFeature Feature = m_pDSet->GetFeature(m_nReadPos);
    while(Feature.IsOk())
    {
        //check if Feature will destroy by Ref Count
        wxGISGeometry Geom = Feature.GetGeometry();
        if(Geom.IsOk())
        {
            if(poCT != NULL)
            {
                Geom = Geom.Copy();
                Geom.Project(poCT);
            }

            wxGISSpatialTreeData *pData(NULL);
            long nFid = Feature.GetFID();
            if(Feature.GetRefData()->GetRefCount() > 1)
                pData = new wxGISSpatialTreeData(Geom, nFid);//appologise the feature is buffered in m_pDSet
            else
                pData = new wxGISSpatialTreeData(Geom.Copy(), nFid);

            Insert(pData);
            m_paCachedData.push_back(pData);

            if(nItemCounter == m_nPreloadItemCount)
            {
                wxGISSpatialTreeCursor cursor;
                WX_APPEND_ARRAY(cursor, m_paCachedData);
                /*for(wxVector<wxGISSpatialTreeData*>::const_iterator it = m_paCachedData.begin(); it != m_paCachedData.end(); ++it)
                    cursor.push_back(*it);*/
                m_pDSet->PostEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, cursor));
                nItemCounter = 0;
                m_paCachedData.clear();
            }
        }

        m_nReadPos++;
        nItemCounter++;

        if (GetThread()->TestDestroy())
        {
            saIgnoredFields.Clear();
            m_pDSet->SetIgnoredFields(saIgnoredFields);

	        if(m_pTrackCancel)
	        {
		        m_pTrackCancel->PutMessage(_("Cancel"), -1, enumGISMessageInfo);
            }

           	if(pProgress)
            {
                pProgress->ShowProgress(false);
            }

            if(poCT)
                OCTDestroyCoordinateTransformation(poCT);


            m_pTrackCancel = NULL;

            return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
        }
        else if(pProgress)
        {
            pProgress->SetValue(m_nReadPos);
        }

        Feature = m_pDSet->Next();
    }

    if(nItemCounter > 0)
    {
        wxGISSpatialTreeCursor cursor;
        WX_APPEND_ARRAY(cursor, m_paCachedData);
        /*for(wxVector<wxGISSpatialTreeData*>::const_iterator it = m_paCachedData.begin(); it != m_paCachedData.end(); ++it)
            cursor.push_back(*it);*/
        m_pDSet->PostEvent(new wxFeatureDSEvent(wxDS_FEATURES_ADDED, cursor));
    }

    if(pProgress)
    {
        pProgress->ShowProgress(false);
    }

	if(m_pTrackCancel)
	{
		m_pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageInfo);
        m_pTrackCancel = NULL;
    }

    saIgnoredFields.Clear();
    m_pDSet->SetIgnoredFields(saIgnoredFields);

    m_pDSet->SetCached(m_nReadPos == nFeaturesCount);

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

    m_bIsLoaded = true;

	return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success

}

bool wxGISSpatialTree::CreateAndRunLoadGeometryThread(void)
{
    if(CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        wxLogError(wxT("Could not create the worker thread!"));
        return false;
    }
    // go!
    if(GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(wxT("Could not run the worker thread!"));
        return false;
    }

    return true;
}

void wxGISSpatialTree::DestroyLoadGeometryThread(void)
{
    if(IsLoading())
    {
        //if (m_pTrackCancel)
        //    m_pTrackCancel->Cancel();
        GetThread()->Delete();//Wait();//
    }
}

void wxGISSpatialTree::Insert(const wxGISGeometry &Geom, long nFID)
{
    Insert(new wxGISSpatialTreeData(Geom, nFID));
}

void wxGISSpatialTree::Change(const wxGISGeometry &Geom, long nFID)
{
	Remove(nFID);
    Insert(Geom, nFID);
}

//-----------------------------------------------------------------------------
// wxGISRTreeNode
//-----------------------------------------------------------------------------

wxGISRTreeNode::wxGISRTreeNode(unsigned short nMinChildItems, unsigned short nMaxChildItems, wxGISSpatialTreeData *pData)
{
    m_nMinChildItems = nMinChildItems;
    m_nMaxChildItems = nMaxChildItems;
    m_pData = pData;

    if(m_pData && m_pData->GetGeometry().IsOk())
    {
        wxGISGeometry Geom = m_pData->GetGeometry();
        if(Geom.IsOk())
        {
            m_Env = Geom.GetEnvelope();
            m_dfArea = fabs((m_Env.MaxX - m_Env.MinX) * (m_Env.MaxY - m_Env.MinY));
        }
    }
}

wxGISRTreeNode::~wxGISRTreeNode()
{
    Delete();
}

bool wxGISRTreeNode::GetHasLeaves() const
{
    return m_bHasLeaves;
}

void wxGISRTreeNode::SetHasLeaves(bool bHasLeaves)
{
    m_bHasLeaves = bHasLeaves;
}

size_t wxGISRTreeNode::GetCount() const
{
    return m_paNodes.size();
}

wxGISRTreeNode* wxGISRTreeNode::GetNode(size_t nIndex)
{
    wxCHECK_MSG(nIndex < m_paNodes.size(), NULL, wxT("The index out of array bounds"));

    return m_paNodes[nIndex];
}

void wxGISRTreeNode::PutNode(wxGISRTreeNode* pNode)
{
    if(m_paNodes.empty())
        m_paNodes.reserve(m_nMinChildItems);

    m_paNodes.push_back(pNode);
}

double wxGISRTreeNode::GetArea() const
{
    return m_dfArea;
}

wxGISRTreeNode* wxGISRTreeNode::ChooseSubtree(const OGREnvelope &Env, enumChooseSubteeType eType)
{
    switch(eType)
    {
    case PARTIAL_SORT:
        std::partial_sort(m_paNodes.begin(), m_paNodes.begin() + RTREE_CHOOSE_SUBTREE_P, m_paNodes.end(), SortBoundedItemsByAreaEnlargement(Env));
        return *std::min_element(m_paNodes.begin(), m_paNodes.begin() + RTREE_CHOOSE_SUBTREE_P, SortBoundedItemsByOverlapEnlargement(Env));
    case SORT_ENV:
        return *std::min_element(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByOverlapEnlargement(Env));
    case SORT_AREA:
        return *std::min_element(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByAreaEnlargement(Env));
    default:
        return NULL;
    }
}

void wxGISRTreeNode::Delete()
{
    for(size_t nIndex = 0; nIndex < GetCount(); ++nIndex)
    {
        wgDELETE(m_paNodes[nIndex], Delete());
    }

    m_paNodes.clear();

    wxDELETE(m_pData);
}

void wxGISRTreeNode::StretchBounds(const OGREnvelope &Env)
{
    m_Env.Merge(Env);
    m_dfArea = fabs((m_Env.MaxX - m_Env.MinX) * (m_Env.MaxY - m_Env.MinY));
}

void wxGISRTreeNode::UpdateBounds(void)
{
    m_Env.MaxX = m_Env.MinX = m_Env.MaxY = m_Env.MinY = 0.0;

    for(size_t i = 0; i < m_paNodes.size(); ++i)
    {
        m_Env.Merge(m_paNodes[i]->GetBounds());
    }

    m_dfArea = fabs((m_Env.MaxX - m_Env.MinX) * (m_Env.MaxY - m_Env.MinY));
}

OGREnvelope wxGISRTreeNode::GetBounds() const
{
    return m_Env;
}

wxGISSpatialTreeData* wxGISRTreeNode::GetData() const
{
    return m_pData;
}

void wxGISRTreeNode::RemoveFarestItems(size_t p, wxVector<wxGISRTreeNode*> &removed_items)
{
    // RI2: Sort the items in increasing order of their distances
    // computed in RI1
    std::partial_sort(m_paNodes.begin(), m_paNodes.end() - p, m_paNodes.end(), SortBoundedItemsByDistanceFromCenter(m_Env));

    // RI3.A: Remove the last p items from N
    removed_items.assign(m_paNodes.end() - p, m_paNodes.end());
    m_paNodes.erase(m_paNodes.end() - p, m_paNodes.end());
}

wxGISRTreeNode* wxGISRTreeNode::Split()
{
    wxGISRTreeNode* pNewNode = new wxGISRTreeNode(m_nMinChildItems, m_nMaxChildItems);
	pNewNode->SetHasLeaves(GetHasLeaves());

	const size_t nItems = GetCount();

	size_t split_axis = 3, split_edge = 0, split_index = 0;
	int split_margin = 0;
    const size_t nDistributionCount = GetCount() - 2 * m_nMinChildItems + 1;


    // these should always hold true
    wxASSERT(nItems == m_nMaxChildItems + 1);
	wxASSERT(m_nMinChildItems + nDistributionCount - 1 <= nItems);

    // S1: Invoke ChooseSplitAxis to determine the axis,
    // perpendicular to which the split 1s performed
    // S2: Invoke ChooseSplitIndex to determine the best
    // distribution into two groups along that axis

    // NOTE: We don't compare against node->bound, so it gets overwritten
    // at the end of the loop

    // CSA1: For each axis
    // CSA1X
    SPLIT_DIR dirX = GetSplitDirection(true, nDistributionCount);
    // CSA1Y
    SPLIT_DIR dirY = GetSplitDirection(false, nDistributionCount);
    // CSA2: Choose the axis with the minimum S as split axis
    bool bIsX;

    if(dirX.split_margin > dirY.split_margin )
    {
        split_margin = dirX.split_margin;
        split_edge = dirX.split_edge;
        split_index	= dirX.split_index;
        bIsX = true;
    }
    else
    {
        split_margin = dirY.split_margin;
        split_edge = dirY.split_edge;
        split_index	= dirY.split_index;
        bIsX = false;
    }

    // S3: Distribute the items into two groups

    // ok, we're done, and the best distribution on the selected split
    // axis has been recorded, so we just have to recreate it and
    // return the correct index

    if (split_edge == 0)
    {
        std::sort(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByEdge(bIsX, true));
    }
	// only reinsert the sort key if we have to
	else
    {
        std::sort(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByEdge(bIsX, false));
	}
    // distribute the end of the array to the new node, then erase them from the original node
    pNewNode->m_paNodes.assign(m_paNodes.begin() + split_index, m_paNodes.end());
    m_paNodes.erase(m_paNodes.begin() + split_index, m_paNodes.end());

    // adjust the bounding box for each 'new' node
    UpdateBounds();
    pNewNode->UpdateBounds();

    return pNewNode;
}

wxGISRTreeNode::SPLIT_DIR wxGISRTreeNode::GetSplitDirection(const bool bIsX, size_t nDistributionCount)
{
    // initialize per-loop items
    int margin = 0;
    double overlap = 0, dist_area, dist_overlap;
    std::size_t dist_edge = 0, dist_index = 0;

    dist_area = dist_overlap = std::numeric_limits<double>::max();


    // Sort the items by the lower then by the upper
    // edge of their bounding box on this particular axis and
    // determine all distributions as described . Compute S. the
    // sum of all margin-values of the different
    // distributions

    // lower edge == 0, upper edge = 1
    for (size_t edge = 0; edge < 2; edge++)
    {
        // sort the items by the correct key (upper edge, lower edge)
        if (edge == 0)
            std::sort(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByEdge(bIsX, true));
        else
            std::sort(m_paNodes.begin(), m_paNodes.end(), SortBoundedItemsByEdge(bIsX, false));

        // Distributions: pick a point m in the middle of the thing, call the left
        // R1 and the right R2. Calculate the bounding box of R1 and R2, then
        // calculate the margins. Then do it again for some more points
        for (std::size_t k = 0; k < nDistributionCount; ++k)
        {
            double area = 0;

            // calculate bounding box of R1
            OGREnvelope R1, R2;
            for(size_t ii = 0; ii < m_nMinChildItems + k; ++ii)
            {
                R1.Merge(m_paNodes[ii]->GetBounds());
            }

            for(size_t ii = m_nMinChildItems + k; ii < GetCount(); ++ii)
            {
                R2.Merge(m_paNodes[ii]->GetBounds());
            }


            // calculate the three values
            double dfDeltaX1, dfDeltaY1, dfDeltaX2, dfDeltaY2;
            dfDeltaX1 = R1.MaxX - R1.MinX;
            dfDeltaY1 = R1.MaxY - R1.MinY;
            dfDeltaX2 = R2.MaxX - R2.MinX;
            dfDeltaY2 = R2.MaxY - R2.MinY;
            margin += dfDeltaX1 + dfDeltaY1 + dfDeltaX2 + dfDeltaY2;
            area += fabs(dfDeltaX1 * dfDeltaY1) + fabs(dfDeltaX2 * dfDeltaY2);

            R1.Intersect(R2);
            overlap = fabs(R1.MaxX - R1.MinX * R1.MaxY - R1.MinY);

            // CSI1: Along the split axis, choose the distribution with the
            // minimum overlap-value. Resolve ties by choosing the distribution
            // with minimum area-value.
            if (overlap < dist_overlap || (overlap == dist_overlap && area < dist_area))
            {
                // if so, store the parameters that allow us to recreate it at the end
                dist_edge = 	edge;
                dist_index = 	m_nMinChildItems + k;
                dist_overlap = 	overlap;
                dist_area = 	area;
            }
        }
    }

    SPLIT_DIR ret = {margin, dist_edge, dist_index};
    return ret;
}

void wxGISRTreeNode::Search(wxGISSpatialTreeCursor &Cursor, const OGREnvelope& env)
{
    for(size_t i = 0; i < GetCount(); ++i)
    {
        if(m_paNodes[i]->GetBounds().Intersects(env))
        {
            if(m_paNodes[i]->GetCount() > 0)
            {
                m_paNodes[i]->Search(Cursor, env);
            }
            else
            {
                Cursor.push_back(m_paNodes[i]->GetData());
            }
        }
    }
}

void wxGISRTreeNode::GetAll(wxGISSpatialTreeCursor &Cursor)
{
    for(size_t i = 0; i < GetCount(); ++i)
    {
        if(m_paNodes[i]->GetCount() > 0)
        {
            m_paNodes[i]->GetAll(Cursor);
        }
        else
        {
            Cursor.push_back(m_paNodes[i]->GetData());
        }
    }
}

bool wxGISRTreeNode::Remove(long nFID)
{
    for(size_t i = 0; i < m_paNodes.size(); ++i)
    {
        if(m_paNodes[i]->GetCount() > 0)
        {
            if(m_paNodes[i]->Remove(nFID))
            {
                m_paNodes.erase(m_paNodes.begin() + i);
                if(m_paNodes.empty())
                {
                    delete this;
                    return true;
                }
				return false;
            }
        }
        else
        {
            if(m_paNodes[i]->GetData()->GetFID() == nFID)
            {
                m_paNodes.erase(m_paNodes.begin() + i);
                if(m_paNodes.empty())
                {
                    delete this;
                    return true;
                }
				return false;
            }
        }
    }
    return false;
}

bool wxGISRTreeNode::HasFID(long nFID) const
{
    for(size_t i = 0; i < m_paNodes.size(); ++i)
    {
        if(m_paNodes[i]->GetCount() > 0)
        {
            if(m_paNodes[i]->HasFID(nFID))
                return true;
        }
        else
        {
            if(m_paNodes[i]->GetData()->GetFID() == nFID)
            {
                return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// wxGISRTree
//-----------------------------------------------------------------------------

wxGISRTree::wxGISRTree(wxGISFeatureDataset* pDSet, unsigned short nMaxChildItems, unsigned short nMinChildItems) : wxGISSpatialTree(pDSet)
{
   	m_nMaxChildItems = nMaxChildItems;
	m_nMinChildItems = nMinChildItems;

    wxASSERT(1 <= nMinChildItems && nMinChildItems <= nMaxChildItems / 2);

    m_pRoot = NULL;
}

wxGISRTree::~wxGISRTree(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxDELETE(m_pRoot);
}

wxGISRTreeNode* wxGISRTree::ChooseSubtree(wxGISRTreeNode* pNode, const OGREnvelope &Env)
{
	// If the child pointers in N point to leaves
	if(pNode->GetNode(0)->GetHasLeaves())
	{
		// determine the minimum overlap cost
		if (m_nMaxChildItems > (RTREE_CHOOSE_SUBTREE_P * 2) / 3  && pNode->GetCount() > RTREE_CHOOSE_SUBTREE_P)
		{
            return pNode->ChooseSubtree(Env, wxGISRTreeNode::PARTIAL_SORT);
        }
        return pNode->ChooseSubtree(Env, wxGISRTreeNode::SORT_ENV);
	}

    return pNode->ChooseSubtree(Env, wxGISRTreeNode::SORT_AREA);
}

void wxGISRTree::Insert(wxGISSpatialTreeData* pData)
{
	wxCriticalSectionLocker locker(m_CritSect);

    wxGISRTreeNode* pNode = new wxGISRTreeNode(m_nMinChildItems, m_nMaxChildItems, pData );
	if (!m_pRoot)
	{
		m_pRoot = new wxGISRTreeNode(m_nMinChildItems, m_nMaxChildItems);
		m_pRoot->SetHasLeaves(true);
	}
    InsertInternal(pNode, m_pRoot);
}

wxGISRTreeNode* wxGISRTree::InsertInternal(wxGISRTreeNode* pInsertNode, wxGISRTreeNode * pStartNode, bool bIsFirstInsert)
{
    wxCHECK_MSG(pInsertNode && pStartNode, NULL, wxT("The one or both of input pointers are null"));
	// I4: Adjust all covering rectangles in the insertion path
	// such that they are minimum bounding boxes
	// enclosing the children rectangles
	pStartNode->StretchBounds(pInsertNode->GetBounds());


	// CS2: If we're at a leaf, then use that level
	if(pStartNode->GetHasLeaves())
	{
		// I2: If N has less than M items, accommodate E in N
		pStartNode->PutNode(pInsertNode);
	}
	else
	{
		// I1: Invoke ChooseSubtree. with the level as a parameter,
		// to find an appropriate node N, m which to place the
		// new leaf E

		// of course, this already does all of that recursively. we just need to
		// determine whether we need to split the overflow or not
		wxGISRTreeNode* tmp_node = InsertInternal( pInsertNode, ChooseSubtree(pStartNode, pInsertNode->GetBounds()), bIsFirstInsert );

		if (!tmp_node)
			return NULL;

		// this gets joined to the list of items at this level
		pStartNode->PutNode(tmp_node);
	}


    // If N has M+1 items. invoke OverflowTreatment with the
    // level of N as a parameter [for reinsertion or split]
    if (pStartNode->GetCount() > m_nMaxChildItems )
    {

        // I3: If OverflowTreatment was called and a split was
        // performed, propagate OverflowTreatment upwards
        // if necessary

        // This is implicit, the rest of the algorithm takes place in there
        return OverflowTreatment(pStartNode, bIsFirstInsert);
    }

    return NULL;
}

// TODO: probably could just merge this in with InsertInternal()
wxGISRTreeNode* wxGISRTree::OverflowTreatment(wxGISRTreeNode* pNode, bool bIsFirstInsert)
{
    // OT1: If the level is not the root level AND this is the first
    // call of OverflowTreatment in the given level during the
    // insertion of one data rectangle, then invoke Reinsert
    if (pNode != m_pRoot && bIsFirstInsert)
    {
        Reinsert(pNode);
        return NULL;
    }

    wxGISRTreeNode* pSplitNode = Split(pNode);

    // If OverflowTreatment caused a split of the root, create a new root
    if (pNode == m_pRoot)
    {
        wxGISRTreeNode* pNewRoot = new wxGISRTreeNode(m_nMinChildItems, m_nMaxChildItems);
        pNewRoot->SetHasLeaves(false);

        // reserve memory
        pNewRoot->PutNode(m_pRoot);
        pNewRoot->PutNode(pSplitNode);

        // Do I4 here for the new root item
        pNewRoot->UpdateBounds();

        // and we're done
        m_pRoot = pNewRoot;
        return NULL;
    }

    // propagate it upwards
    return pSplitNode;
}

// This routine is used to do the opportunistic reinsertion that the
// R* algorithm calls for
void wxGISRTree::Reinsert(wxGISRTreeNode* pNode)
{
    wxVector<wxGISRTreeNode*> removed_items;

    const size_t nItems = pNode->GetCount();
    size_t val = (size_t)((double)nItems * RTREE_REINSERT_P);
    const size_t p = val > 0 ? val : 1;

    // RI1 For all M+l items of a node N, compute the distance
    // between the centers of their rectangles and the center
    // of the bounding rectangle of N
    wxASSERT(nItems == m_nMaxChildItems + 1);

    // RI2: Sort the items in increasing order of their distances
    // computed in RI1
    pNode->RemoveFarestItems(p, removed_items);

    // RI3.B: adjust the bounding rectangle of N
    pNode->UpdateBounds();

    // RI4: In the sort, defined in RI2, starting with the
    // minimum distance (= close reinsert), invoke Insert
    // to reinsert the items
    for(wxVector<wxGISRTreeNode*>::iterator it = removed_items.begin(); it != removed_items.end(); ++it)
    {
        InsertInternal( *it, m_pRoot, false);
    }
}

// This returns a node, which should be added to the items of the
// passed node's parent

wxGISRTreeNode* wxGISRTree::Split(wxGISRTreeNode* pNode)
{
    return pNode->Split();
}

void wxGISRTree::Remove(long nFID)
{
	wxCriticalSectionLocker locker(m_CritSect);

	if (m_pRoot && m_pRoot->Remove(nFID))
		m_pRoot = NULL;
}

bool wxGISRTree::HasFID(long nFID) const
{
	if (!m_pRoot)
		return false;
    return m_pRoot->HasFID(nFID);
}

void wxGISRTree::RemoveAll()
{
	wxCriticalSectionLocker locker(m_CritSect);
	if (!m_pRoot)
		return;
    // Delete all existing nodes
    m_pRoot->Delete();
}

wxGISSpatialTreeCursor wxGISRTree::Search(const OGREnvelope& env)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if (!m_pRoot)
		return wxNullSpatialTreeCursor;

    wxGISSpatialTreeCursor ret;

    if(env.IsInit())
    {
        m_pRoot->Search(ret, env);
    }
    else
    {
        m_pRoot->GetAll(ret);
    }
    return ret;
}

//-----------------------------------------------------------------------------
// GetGeometryBoundsFunc
//-----------------------------------------------------------------------------

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	wxGISSpatialTreeData* pTreeItem = (wxGISSpatialTreeData*)hFeature;
    if(!pTreeItem)
		return;

    wxGISGeometry Geom = pTreeItem->GetGeometry();
    if(Geom.IsOk())
    {
        OGREnvelope Env = Geom.GetEnvelope();
	    if(IsDoubleEquil(Env.MinX, Env.MaxX))
		    Env.MaxX += DELTA;
	    if(IsDoubleEquil(Env.MinY, Env.MaxY))
		    Env.MaxY += DELTA;

	    pBounds->minx = Env.MinX;
	    pBounds->maxx = Env.MaxX;
	    pBounds->miny = Env.MinY;
	    pBounds->maxy = Env.MaxY;
    }
}


//-----------------------------------------------------------------------------
// wxGISQuadTree
//-----------------------------------------------------------------------------

wxGISQuadTree::wxGISQuadTree(wxGISFeatureDataset* pDSet) : wxGISSpatialTree(pDSet)
{
    CreateQuadTree();
}

wxGISQuadTree::~wxGISQuadTree(void)
{
    DestroyQuadTree();
}

void wxGISQuadTree::CreateQuadTree()
{
    //create quadtree using full extent of zone projected SRS or
    //the whole world for Geographic
/*    m_Envelope.MinX = -360;
    m_Envelope.MinY = -180;
    m_Envelope.MaxX = 360;
    m_Envelope.MaxY = 180;

    if(SpatRef.IsOk() && !SpatRef.IsSame(m_pDSet->GetSpatialReference()))
    {
        if(!SpatRef->IsGeographic())
        {
            m_Envelope = m_pDSet->GetEnvelope();
            wxGISGeometry newGeom = EnvelopeToGeometry(m_Envelope, m_pDSet->GetSpatialReference());
            if(newGeom.IsOk() && newGeom.Project(SpatRef))
            {
                m_Envelope = newGeom.GetEnvelope();
            }
            else
            {
                m_Envelope.MinX = -60000000;
                m_Envelope.MinY = -60000000;
                m_Envelope.MaxX = 60000000;
                m_Envelope.MaxY = 60000000;
            }
        }
    }
    else
    {
        if(m_pDSet->GetSpatialReference().IsOk() && !m_pDSet->GetSpatialReference()->IsGeographic())
        {
            wxGISGeometry newGeom = EnvelopeToGeometry(m_Envelope, wxGISSpatialReference(wxString(SRS_WKT_WGS84)));
            if(newGeom.Project(m_pDSet->GetSpatialReference()))
            {
                m_Envelope = newGeom.GetEnvelope();
            }
            else
            {
                m_Envelope = m_pDSet->GetEnvelope();
                IncreaseEnvelope(m_Envelope, 100);
            }
        }
    }

    CPLRectObj Rect = {m_Envelope.MinX, m_Envelope.MinY, m_Envelope.MaxX, m_Envelope.MaxY};
*/
#define MAX_QT_EXT 200000000
    CPLRectObj Rect = {-MAX_QT_EXT, -MAX_QT_EXT, MAX_QT_EXT, MAX_QT_EXT};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);
}

void wxGISQuadTree::DestroyQuadTree()
{
	if(m_pQuadTree)
    {
		CPLQuadTreeDestroy(m_pQuadTree);
        m_pQuadTree = NULL;
    }

    WX_CLEAR_ARRAY(m_Cursor);
    /*wxGISSpatialTreeCursor::iterator iter;
    for(iter = m_Cursor.begin(); iter != m_Cursor.end(); ++iter)
    {
        wxDELETE(*iter);
    }*/
}

void wxGISQuadTree::Insert(wxGISSpatialTreeData* pData)
{
    if(!pData)
        return;
    if(!pData->GetGeometry().IsOk())
        return;

    m_Envelope.Merge(pData->GetGeometry().GetEnvelope());
	CPLQuadTreeInsert(m_pQuadTree, (void*)pData);
	m_Cursor.push_back(pData);
}

void wxGISQuadTree::Remove(long nFID)
{
    wxGISSpatialTreeCursor::iterator iter;
    for(iter = m_Cursor.begin(); iter != m_Cursor.end(); ++iter)
    {
		wxGISSpatialTreeData* item = *iter;
        if(item && item->GetFID() == nFID)
        {
            item->SetGeometry(wxNullGeometry);
            item->SetFID(wxNOT_FOUND);
        }
    }
}

bool wxGISQuadTree::HasFID(long nFID) const
{
    wxGISSpatialTreeCursor::const_iterator iter;
    for(iter = m_Cursor.begin(); iter != m_Cursor.end(); ++iter)
    {
		wxGISSpatialTreeData* item = *iter;
        if(item && item->GetFID() == nFID)
            return true;
    }
    return false;
}

void wxGISQuadTree::RemoveAll()
{
    DestroyQuadTree();
    CreateQuadTree();
}

wxGISSpatialTreeCursor wxGISQuadTree::Search(const OGREnvelope& Env)
{
	wxCriticalSectionLocker locker(m_CritSect);

    bool bContains(false);
	if(Env.IsInit())
	{
		if(!IsDoubleEquil(m_Envelope.MaxX, Env.MaxX) || !IsDoubleEquil(m_Envelope.MaxY, Env.MaxY) || !IsDoubleEquil(m_Envelope.MinX, Env.MinX) || !IsDoubleEquil(m_Envelope.MinY, Env.MinY))
			bContains = m_Envelope.Contains(Env) != 0;
	}

	if(bContains && !IsLoading())
	{
        int nItemCount(0);
        CPLRectObj AOI = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
		wxGISSpatialTreeData** ppData = (wxGISSpatialTreeData**)CPLQuadTreeSearch(m_pQuadTree, &AOI, &nItemCount);

        wxGISSpatialTreeCursor retCursor;
        for(int i = 0; i < nItemCount; ++i)
        {
            retCursor.push_back(ppData[i]);
        }
		return retCursor;
	}
	else
	{
        if(m_bIsLoaded)
        {
      //      wxGISSpatialTreeCursor retCursor;
      //      WX_APPEND_ARRAY(retCursor, m_Cursor);
		    //return retCursor;
      //  }
      //  else
      //  {
            return m_Cursor;
        }
	}

    return wxNullSpatialTreeCursor;
}



