/******************************************************************************
 * Project:  wxGIS (GIS)
 * Purpose:  differrent classes for smart pointers, point connections, etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 20014 Dmitry Baryshnikov
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

#include "wxgis/core/core.h"

#include <wx/vector.h>

/** @class wxGISConnectionPointContainer

    The class for event connection store.

    @library{core}
*/

class WXDLLIMPEXP_GIS_CORE wxGISConnectionPointContainer
{
public:
    wxGISConnectionPointContainer(void);
    virtual ~wxGISConnectionPointContainer(void);
    virtual long Advise(wxEvtHandler* pEvtHandler);
    virtual void Unadvise(long nCookie);
protected:
	virtual void AddEvent(const wxEvent &event);
    virtual void PostEvent(wxEvent *event);
protected:
	wxVector<wxEvtHandler*> m_pPointsArray;
    wxCriticalSection m_CritSectEvt;
};

/** @class wxGISPointer

    A simple smart pointer class.

    @library{core}
*/
class WXDLLIMPEXP_GIS_CORE wxGISPointer
{
public:
    wxGISPointer();
    virtual ~wxGISPointer(void);
    //ref count
	virtual wxInt32 Reference(void);
	virtual wxInt32 Dereference(void);
	virtual wxInt32 Release(void);
protected:
	wxInt32 m_RefCount;
};

/** @class wxGISPointerHolder
  * 
  * A wxGISPointer holder. Dereference on distruct.
  *
  * @library{core}
  */
class WXDLLIMPEXP_GIS_CORE wxGISPointerHolder
{
public:
    wxGISPointerHolder(wxGISPointer* pPointer);
    virtual ~wxGISPointerHolder(void);
protected:
    wxGISPointer* m_pPointer;
};