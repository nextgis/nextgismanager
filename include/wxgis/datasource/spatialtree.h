/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Various Spatial Tree classes
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
#pragma once

#include "wxgis/datasource/gdalinh.h"

#include "wx/thread.h"
#include <wx/list.h>

class wxGISFeatureDataset;

/** @class wxGISSpatialTreeData
    
    The data included geography and feature identificator (FID) stored in spatial tree.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISSpatialTreeData
{
public:
    wxGISSpatialTreeData(const wxGISGeometry &Geom = wxNullGeometry, long nFID = wxNOT_FOUND);
    virtual ~wxGISSpatialTreeData();
	virtual void SetFID(long nFID);
	virtual long GetFID(void) const;
	virtual wxGISGeometry GetGeometry(void) const;
	virtual void SetGeometry(const wxGISGeometry &oGeom);
protected:
    wxGISGeometry m_Geom;
    long m_nFID;    
};

WX_DEFINE_ARRAY_WITH_DECL_PTR(wxGISSpatialTreeData*, wxGISSpatialTreeCursor, class WXDLLIMPEXP_GIS_DS);

extern WXDLLIMPEXP_DATA_GIS_DS(wxGISSpatialTreeCursor) wxNullSpatialTreeCursor;

/** @class wxGISSpatialTree
    
    A abstract class for spatial tree implementations.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISSpatialTree : public wxThreadHelper
{
public:
	wxGISSpatialTree(wxGISFeatureDataset* pDSet);
	virtual ~wxGISSpatialTree(void);
    virtual bool Load(const wxGISSpatialReference &SpatRef = wxNullSpatialReference, ITrackCancel* const pTrackCancel = NULL);
    virtual bool IsLoading(void) const;
    virtual void CancelLoading();

    virtual void Insert(const wxGISGeometry &Geom, long nFID);
    virtual void Remove(long nFID) = 0;
    virtual void RemoveAll(void) = 0;
    virtual void Change(const wxGISGeometry &Geom, long nFID);
    virtual wxGISSpatialTreeCursor Search(const OGREnvelope& env) = 0;  
    virtual void Insert(wxGISSpatialTreeData* pData) = 0;
    virtual bool HasFID(long nFID) const = 0;
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunLoadGeometryThread(void);
    void DestroyLoadGeometryThread(void);
protected:
    wxGISFeatureDataset* m_pDSet;
    long m_nReadPos;
    short m_nPreloadItemCount;
protected:
    bool m_bIsLoaded;
   	wxCriticalSection m_CritSect;
    wxGISSpatialReference m_SpatialReference;
    ITrackCancel* m_pTrackCancel;
};

/** @fn wxGISSpatialTree* CreateSpatialTree(void)
    
    Global function to create spatial tree. The caller need to delete returned pointer themselthes.

    @library{datasource}
 */
WXDLLIMPEXP_GIS_DS wxGISSpatialTree* CreateSpatialTree(wxGISFeatureDataset *pDS);

// R* tree parameters

#define RTREE_REINSERT_P 0.30
#define RTREE_CHOOSE_SUBTREE_P 32

/** @class wxGISRTreeNode
    
    The node for storing other nodes or geodata.

    @library{datasource}
*/

class wxGISRTreeNode
{
    friend class wxGISRTree;
    
    enum enumChooseSubteeType{
        PARTIAL_SORT = 1,
        SORT_ENV = 2,
        SORT_AREA = 3
    };
public:
    wxGISRTreeNode(unsigned short nMinChildItems = 32, unsigned short nMaxChildItems = 64, wxGISSpatialTreeData *pData = NULL);
    ~wxGISRTreeNode();
    
    wxGISSpatialTreeData* GetData() const;
    
    typedef struct _split_dir{
        int split_margin;
        size_t split_edge;
        size_t split_index;
    } SPLIT_DIR;
protected:  
    void Search(wxGISSpatialTreeCursor &Cursor, const OGREnvelope& env); 
    void GetAll(wxGISSpatialTreeCursor &Cursor); 
    bool Remove(long nFID);
    bool HasFID(long nFID) const;
    double GetArea() const;
    void StretchBounds(const OGREnvelope &Env);
    wxGISRTreeNode* ChooseSubtree(const OGREnvelope &Env, enumChooseSubteeType eType);
    OGREnvelope GetBounds() const;
    size_t GetCount() const;
    bool GetHasLeaves() const;
    void SetHasLeaves(bool bHasLeaves);
    wxGISRTreeNode* GetNode(size_t nIndex);
    void PutNode(wxGISRTreeNode* pNode);
    void UpdateBounds(void);
    wxGISRTreeNode* Split();
    SPLIT_DIR GetSplitDirection(const bool bIsX, size_t nDistributionCount);
    void RemoveFarestItems(size_t p, wxVector<wxGISRTreeNode*> &removed_items);
    void Delete();
protected:
    struct SortBoundedItemsByAreaEnlargement : public std::binary_function< const wxGISRTreeNode * const, const wxGISRTreeNode * const, bool >
    {
	    double m_dfArea;
	    explicit SortBoundedItemsByAreaEnlargement(const OGREnvelope &Env)
        {
            m_dfArea = fabs((Env.MaxX - Env.MinX) * (Env.MaxY - Env.MinY));
        }

	    bool operator() (const wxGISRTreeNode * const n1, const wxGISRTreeNode * const n2) const 
	    {
		    return m_dfArea - n1->GetArea() < m_dfArea - n2->GetArea();
	    }
    };

    struct SortBoundedItemsByOverlapEnlargement : public std::binary_function< const wxGISRTreeNode * const, const wxGISRTreeNode * const, bool >
    {
        const OGREnvelope m_Env; 
        explicit SortBoundedItemsByOverlapEnlargement(const OGREnvelope &Env) : m_Env(Env) {}

	    bool operator() (const wxGISRTreeNode * const n1, const wxGISRTreeNode * const n2) const 
	    {
            OGREnvelope IntersectEnv = n1->GetBounds();
            IntersectEnv.Intersect(m_Env);
            double dfArea1 = fabs((IntersectEnv.MaxX - IntersectEnv.MinX) * (IntersectEnv.MaxY - IntersectEnv.MinY));
            IntersectEnv = n2->GetBounds();
            IntersectEnv.Intersect(m_Env);
            double dfArea2 = fabs((IntersectEnv.MaxX - IntersectEnv.MinX) * (IntersectEnv.MaxY - IntersectEnv.MinY));
            
		    return dfArea1 < dfArea2;
	    }
    };
    
    struct SortBoundedItemsByDistanceFromCenter : public std::binary_function< const wxGISRTreeNode * const, const wxGISRTreeNode * const, bool >
    {
        double dfX, dfY;
        explicit SortBoundedItemsByDistanceFromCenter(const OGREnvelope &Env)
        {
            dfX = (Env.MaxX - Env.MinX) / 2;
            dfY = (Env.MaxY - Env.MinY) / 2;
        }

        bool operator() (const wxGISRTreeNode * const n1, const wxGISRTreeNode * const n2) const 
        {
            OGREnvelope OtherEnv = n1->GetBounds();
            double dfXO, dfYO;
            dfXO = (OtherEnv.MaxX - OtherEnv.MinX) / 2;
            dfYO = (OtherEnv.MaxY - OtherEnv.MinY) / 2;
            double dfDist1 = /*std::sqrt*/((dfX - dfXO) * (dfX - dfXO) + (dfY - dfYO) * (dfY - dfYO));
            
            OtherEnv = n2->GetBounds();
            dfXO = (OtherEnv.MaxX - OtherEnv.MinX) / 2;
            dfYO = (OtherEnv.MaxY - OtherEnv.MinY) / 2;
            double dfDist2 = /*std::sqrt*/((dfX - dfXO) * (dfX - dfXO) + (dfY - dfYO) * (dfY - dfYO));       
            
            return dfDist1 < dfDist2;
        }
    };
    
    struct SortBoundedItemsByEdge : public std::binary_function< const wxGISRTreeNode * const, const wxGISRTreeNode * const, bool >
    {
        const bool m_bIsX;
        const bool m_bIsMin;
        explicit SortBoundedItemsByEdge (const bool bIsX, const bool bIsMin) : m_bIsX(bIsX), m_bIsMin(bIsMin) {}
        
        bool operator() (const wxGISRTreeNode * const n1, const wxGISRTreeNode * const n2) const 
        {
            OGREnvelope Env1 = n1->GetBounds();
            OGREnvelope Env2 = n2->GetBounds();
            if(m_bIsX)
            {
                if(m_bIsMin)
                {
                    return Env1.MinX < Env2.MinX;
                }
                else //Max
                {
                    return Env1.MaxX < Env2.MaxX;
                }
            }
            else // Y
            {
                if(m_bIsMin)
                {
                    return Env1.MinY < Env2.MinY;
                }
                else //Max
                {
                    return Env1.MaxY < Env2.MaxY;
                }
            }
        }
    };
    
protected:
    unsigned short m_nMinChildItems, m_nMaxChildItems;
    OGREnvelope m_Env;
    double m_dfArea;
    wxVector<wxGISRTreeNode*> m_paNodes;
    wxGISSpatialTreeData* m_pData;
    bool m_bHasLeaves;
};

/** @class wxGISRTree
    
    The wxGIS R-Tree implementation. R-trees are tree data structures used for spatial access methods, i.e., for indexing multi-dimensional information such as geographical coordinates, rectangles or polygons. The R-tree was proposed by Antonin Guttman in 1984 and has found significant use in both theoretical and applied contexts.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISRTree : public wxGISSpatialTree
{
public:
	wxGISRTree(wxGISFeatureDataset* pDSet, unsigned short nMaxChildItems = 64, unsigned short nMinChildItems = 32);
	virtual ~wxGISRTree(void);
    virtual void Remove(long nFID);
    virtual void RemoveAll(void);
    virtual wxGISSpatialTreeCursor Search(const OGREnvelope& env);    
    virtual void Insert( wxGISSpatialTreeData* pData);
    virtual bool HasFID(long nFID) const;
protected:
    virtual wxGISRTreeNode* ChooseSubtree(wxGISRTreeNode* pNode, const OGREnvelope &Env);
    virtual wxGISRTreeNode* InsertInternal(wxGISRTreeNode* pInsertNode, wxGISRTreeNode * pStartNode, bool bIsFirstInsert = true);
    virtual wxGISRTreeNode* OverflowTreatment(wxGISRTreeNode* pNode, bool bIsFirstInsert);
    virtual void Reinsert(wxGISRTreeNode* pNode);
    virtual wxGISRTreeNode* Split(wxGISRTreeNode* pNode);
protected:
    wxGISRTreeNode* m_pRoot;
    unsigned short m_nMaxChildItems;
    unsigned short m_nMinChildItems;
};

#include "cpl_quad_tree.h"

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds);


/** \class wxGISQuadTree

    The wxGIS QuadTree representation.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISQuadTree : public wxGISSpatialTree
{
public:
	wxGISQuadTree(wxGISFeatureDataset* pDSet);
	virtual ~wxGISQuadTree(void);
    virtual void Remove(long nFID);
    virtual void RemoveAll(void);
    virtual wxGISSpatialTreeCursor Search(const OGREnvelope& Env);  
    virtual void Insert(wxGISSpatialTreeData* pData);
    virtual bool HasFID(long nFID) const;
protected:    
    void CreateQuadTree();
    void DestroyQuadTree();
protected:
    CPLQuadTree* m_pQuadTree;
    wxGISSpatialTreeCursor m_Cursor;
    OGREnvelope m_Envelope;
};
