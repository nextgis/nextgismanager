/******************************************************************************
 * Project:  wxGIS
 * Purpose:  DropTarget implementations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Dmitry Baryshnikov
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
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnections.h"

#ifdef wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// wxGISDropTarget
// ----------------------------------------------------------------------------

wxGISDropTarget::wxGISDropTarget(IViewDropTarget *pOwner)
{
    m_pOwner = pOwner;
    wxDataObjectComposite* dataobj = new wxDataObjectComposite();
    dataobj->Add(new wxGISStringDataObject(wxDataFormat(wxGIS_DND_NAME)), true);
    dataobj->Add(new wxFileDataObject());
    //dataobj->Add(new wxGISDecimalDataObject(wxDataFormat(wxT("application/x-vnd.wxgis.gxobject-id"))));
    //dataobj->Add(new wxDataObjectSimple(wxDataFormat(wxT("application/x-vnd.qgis.qgis.uri"))));
    SetDataObject(dataobj);
}

wxGISDropTarget::~wxGISDropTarget(void)
{
}

wxDragResult wxGISDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    return m_pOwner->OnDragOver(x, y, def);
}

wxDragResult wxGISDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    return def;
}

void wxGISDropTarget::OnLeave()
{
    m_pOwner->OnLeave();
}

bool wxGISDropTarget::OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects)
{
    return m_pOwner->OnDropObjects(x, y, GxObjects, wxGetKeyState(WXK_CONTROL), wxGetKeyState(WXK_SHIFT));
}

wxDragResult wxGISDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult)
{
    if ( !GetData() )
        return wxDragNone;

    //wxDragResult dragResult = wxDropTarget::OnData(x, y, defaultDragResult);
    //if ( dragResult == defaultDragResult )
    //{
        wxDataObjectComposite * dataobjComp = static_cast<wxDataObjectComposite *>(GetDataObject());

        wxDataFormat format = dataobjComp->GetReceivedFormat();
        wxDataObject *dataobj = dataobjComp->GetObject(format);

        //TODO:
        //application/x-vnd.wxgis.gxobject-id
        //application/x-vnd.qgis.qgis.uri

        wxDataFormat PrefferedDataFormat(wxGIS_DND_NAME);
        if(format == PrefferedDataFormat)
        {
            wxGISStringDataObject* pStringDataObject = static_cast<wxGISStringDataObject *>(dataobj);
            if(pStringDataObject)
                return OnDropObjects( x, y, pStringDataObject->GetStrings() ) ? defaultDragResult : wxDragNone;
        }

        if(format.GetType() == wxDF_FILENAME)
        {
            //Convert from file path to gxname
            wxFileDataObject* pFileDataObject = static_cast<wxFileDataObject *>(dataobj);
            if(pFileDataObject)
            {
                wxArrayString asSystemPaths = pFileDataObject->GetFilenames();
                wxArrayString asObjects = PathsToNames(asSystemPaths);
                if(asObjects.GetCount() > 0)
                    return OnDropObjects( x, y, asObjects ) ? defaultDragResult : wxDragNone;
            }
        }

        wxFAIL_MSG( "unexpected data object format" );

    //}

    return defaultDragResult;
}

wxArrayString wxGISDropTarget::PathsToNames(const wxArrayString saPaths)
{
    wxArrayString asObjects;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    wxGxObjectContainer* pConnections = wxDynamicCast(pGxCatalog->GetRootItemByType(wxCLASSINFO(wxGxDiscConnections)), wxGxObjectContainer);
    if(pConnections)
    {
        wxGxObjectList::const_iterator iter;
        for(size_t i = 0; i < saPaths.GetCount(); ++i)
        {
            for(iter = pConnections->GetChildren().begin(); iter != pConnections->GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                wxGxObject *searched = current->FindGxObjectByPath(saPaths[i]);
                if(searched)
                {
                    asObjects.Add(searched->GetFullName());
                    break;
                }
            }
        }
    }
    return asObjects;
}

#endif // wxUSE_DRAG_AND_DROP

