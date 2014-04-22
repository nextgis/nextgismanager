/******************************************************************************
 * Project:  wxGIS
 * Purpose:  DropTarget implementations for geoprocesses.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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
#include "wxgis/geoprocessingui/droptarget.h"
#include "wxgis/framework/dataobject.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnections.h"

#ifdef wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// wxGISTaskDropTarget
// ----------------------------------------------------------------------------

wxGISTaskDropTarget::wxGISTaskDropTarget(wxGISToolExecuteView *pOwner)
{
    m_pOwner = pOwner;
    SetDataObject(new wxGISTaskDataObject(wxNOT_FOUND, wxDataFormat(wxGIS_DND_ID)));
}

wxGISTaskDropTarget::~wxGISTaskDropTarget(void)
{
}

wxDragResult wxGISTaskDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    return m_pOwner->OnDragOver(x, y, def);
}

wxDragResult wxGISTaskDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    return m_pOwner->OnEnter(x, y, def);
}

void wxGISTaskDropTarget::OnLeave()
{
    m_pOwner->OnLeave();
}

bool wxGISTaskDropTarget::OnDropObjects(wxCoord x, wxCoord y, long nParentPointer, const wxArrayLong& TaskIds)
{
    return m_pOwner->OnDropObjects(x, y, nParentPointer, TaskIds);
}

wxDragResult wxGISTaskDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult)
{
    if ( !GetData() )
        return wxDragNone;

    wxGISTaskDataObject * pTaskDataObject = static_cast<wxGISTaskDataObject *>(GetDataObject());
    if(pTaskDataObject)
        return OnDropObjects( x, y, pTaskDataObject->GetParentPointer(), pTaskDataObject->GetDecimals() ) ? defaultDragResult : wxDragNone;
   
    wxFAIL_MSG( "unexpected data object format" );

    return defaultDragResult;
}

#endif // wxUSE_DRAG_AND_DROP
