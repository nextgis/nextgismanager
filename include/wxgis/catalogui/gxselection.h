/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSelection class. Selection of IGxObjects in tree or list views
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
#pragma once

#include "wxgis/catalogui/catalogui.h"


/** \class wxGxSelection gxselection.h
 *  \brief The GxObject Selection class.
 */

class WXDLLIMPEXP_GIS_CLU wxGxSelection :
	public wxGISConnectionPointContainer
{
public:
	enum wxGISEnumInitiators
	{
		INIT_ALL = -2,
		INIT_NONE = -1
	} Initiator;
public:
	wxGxSelection(void);
	virtual ~wxGxSelection(void);
	//IGxSelection
	virtual void Select( long nObjectId,  bool appendToExistingSelection, long nInitiator );
    virtual void Select( long nObjectId );
	virtual void Unselect( long nObjectId, long nInitiator );
	virtual void Clear(long nInitiator);
    virtual size_t GetCount(void);
    virtual size_t GetCount(long nInitiator);
    virtual long GetSelectedObjectId(size_t nIndex);
    virtual long GetSelectedObjectId(long nInitiator, size_t nIndex);
    virtual long GetLastSelectedObjectId(void);
    virtual long GetFirstSelectedObjectId(void);
	virtual void SetInitiator(long nInitiator);
    virtual void Do( long nObjectId );
    virtual bool CanRedo();
	virtual bool CanUndo();
    virtual long Redo(int nPos = wxNOT_FOUND);
    virtual long Undo(int nPos = wxNOT_FOUND);
	virtual void RemoveDo(long nObjectId);
    virtual void Reset();
    virtual size_t GetDoSize();
    virtual int GetDoPos(void) const { return m_nPos; };
    virtual long GetDoId(size_t nIndex);
    virtual wxArrayLong GetDoArray(void) const { return m_DoArray; };
protected:
	wxArrayLong m_DoArray;
	int m_nPos;
    bool m_bDoOp;

    long m_pPrevId;
    wxCriticalSection m_DoCritSect, m_CritSect;

	std::map<long, wxArrayLong> m_SelectionMap;
	long m_currentInitiator;
};

/** \class wxGxApplicationBase gxselection.h
    \brief An Interface class for GxApplication.
*/
class WXDLLIMPEXP_GIS_CLU wxGxApplicationBase
{
public:
    wxGxApplicationBase(void);
	virtual ~wxGxApplicationBase(void);
    wxGxSelection* const GetGxSelection(void);
    wxGxSelection* const GetGxSelection(void) const;
    virtual void SetLocation(const wxString &sPath);
    virtual void Undo(int nPos);
    virtual void Redo(int nPos);
protected:
    wxGxSelection* m_pSelection;
};