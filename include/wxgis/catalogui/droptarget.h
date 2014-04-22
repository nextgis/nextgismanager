/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Drag'n'Drop wxGIS implementations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Bishop
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

#define DNDSCROLL 10 

#ifdef wxUSE_DRAG_AND_DROP

#define wxGIS_DND_NAME wxT("application/x-vnd.nextgis_manager.gxobject-name") 

/** @class wxGISDropTarget
    
	Add drag and drop capability for inherited windows.

	@library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISDropTarget : public wxDropTarget
{
public:
    wxGISDropTarget(IViewDropTarget *pOwner);
    virtual ~wxGISDropTarget(void);
    //wxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    virtual void OnLeave();
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult);
    //wxGISDropTarget
    virtual bool OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects);
    static wxArrayString PathsToNames(const wxArrayString saPaths);
private:
    IViewDropTarget *m_pOwner;
private:
    wxDECLARE_NO_COPY_CLASS(wxGISDropTarget);
};

#endif // wxUSE_DRAG_AND_DROP



