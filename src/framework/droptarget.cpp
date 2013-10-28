/******************************************************************************
 * Project:  wxGIS
 * Purpose:  DropTarget implementations.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/framework/droptarget.h"

// ----------------------------------------------------------------------------
// wxGISDropTarget
// ----------------------------------------------------------------------------

wxGISDropTarget::wxGISDropTarget(IViewDropTarget *pOwner)
{
    m_pOwner = pOwner;
    //wxDF_FILENAME 
    //m_dataObject;
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

bool wxGISDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    return m_pOwner->OnDropFiles(x, y, filenames);
}

// ----------------------------------------------------------------------------
// wxGxDataObject
// ----------------------------------------------------------------------------

//wxGxDataObject::wxGxDataObject(int nType, int nSubType)
//{
//    m_nType = nType;
//    m_nSubType = nSubType;
//
//    if ( shape )
//    {
//        // we need to copy the shape because the one we're handled may be
//        // deleted while it's still on the clipboard (for example) - and we
//        // reuse the serialisation methods here to copy it
//        void *buf = malloc(shape->DnDShape::GetDataSize());
//        shape->GetDataHere(buf);
//        m_shape = DnDShape::New(buf);
//
//        free(buf);
//    }
//    else
//    {
//        // nothing to copy
//        m_shape = NULL;
//    }
//
//    // this string should uniquely identify our format, but is otherwise
//    // arbitrary
//    m_formatShape.SetId(shapeFormatId);
//
//    // we don't draw the shape to a bitmap until it's really needed (i.e.
//    // we're asked to do so)
//    m_hasBitmap = false;
//#if wxUSE_METAFILE
//    m_hasMetaFile = false;
//#endif // wxUSE_METAFILE
//}

//wxGxDataObject::~wxGxDataObject()
//{ 
//    //delete m_shape; 
//}
//
//DnDShape *wxGxDataObject::GetShape()
//{
//    DnDShape *shape = m_shape;
//
//    m_shape = (DnDShape *)NULL;
//    m_hasBitmap = false;
//#if wxUSE_METAFILE
//    m_hasMetaFile = false;
//#endif // wxUSE_METAFILE
//
//    return shape;
//}
//
//wxDataFormat wxGxDataObject::GetPreferredFormat(Direction WXUNUSED(dir)) const
//{
//    return m_formatShape;
//}
//
//size_t wxGxDataObject::GetFormatCount(Direction dir) const
//{
//    // our custom format is supported by both GetData() and SetData()
//    size_t nFormats = 1;
//    if ( dir == Get )
//    {
//        // but the bitmap format(s) are only supported for output
//        nFormats += m_dobjBitmap.GetFormatCount(dir);
//
//#if wxUSE_METAFILE
//        nFormats += m_dobjMetaFile.GetFormatCount(dir);
//#endif // wxUSE_METAFILE
//    }
//
//    return nFormats;
//}
//
//void wxGxDataObject::GetAllFormats(wxDataFormat *formats, Direction dir) const
//{
//    formats[0] = m_formatShape;
//    if ( dir == Get )
//    {
//        // in Get direction we additionally support bitmaps and metafiles
//        // under Windows
//        m_dobjBitmap.GetAllFormats(&formats[1], dir);
//
//#if wxUSE_METAFILE
//        // don't assume that m_dobjBitmap has only 1 format
//        m_dobjMetaFile.GetAllFormats(&formats[1 +
//                m_dobjBitmap.GetFormatCount(dir)], dir);
//#endif // wxUSE_METAFILE
//    }
//}
//
//size_t wxGxDataObject::GetDataSize(const wxDataFormat& format) const
//{
//    if ( format == m_formatShape )
//    {
//        return m_shape->GetDataSize();
//    }
//#if wxUSE_METAFILE
//    else if ( m_dobjMetaFile.IsSupported(format) )
//    {
//        if ( !m_hasMetaFile )
//            CreateMetaFile();
//
//        return m_dobjMetaFile.GetDataSize(format);
//    }
//#endif // wxUSE_METAFILE
//    else
//    {
//        wxASSERT_MSG( m_dobjBitmap.IsSupported(format), wxT("unexpected format") );
//
//        if ( !m_hasBitmap )
//            CreateBitmap();
//
//        return m_dobjBitmap.GetDataSize();
//    }
//}
//
//bool wxGxDataObject::GetDataHere(const wxDataFormat& format, void *pBuf) const
//{
//    if ( format == m_formatShape )
//    {
//        m_shape->GetDataHere(pBuf);
//
//        return true;
//    }
//#if wxUSE_METAFILE
//    else if ( m_dobjMetaFile.IsSupported(format) )
//    {
//        if ( !m_hasMetaFile )
//            CreateMetaFile();
//
//        return m_dobjMetaFile.GetDataHere(format, pBuf);
//    }
//#endif // wxUSE_METAFILE
//    else
//    {
//        wxASSERT_MSG( m_dobjBitmap.IsSupported(format), wxT("unexpected format") );
//
//        if ( !m_hasBitmap )
//            CreateBitmap();
//
//        return m_dobjBitmap.GetDataHere(pBuf);
//    }
//}
//
//bool wxGxDataObject::SetData(const wxDataFormat& format, size_t WXUNUSED(len), const void *buf)
//{
//    wxCHECK_MSG( format == m_formatShape, false, wxT( "unsupported format") );
//
//    delete m_shape;
//    m_shape = DnDShape::New(buf);
//
//    // the shape has changed
//    m_hasBitmap = false;
//
//#if wxUSE_METAFILE
//    m_hasMetaFile = false;
//#endif // wxUSE_METAFILE
//
//    return true;
//}
//
//#if wxUSE_METAFILE
//
//void wxGxDataObject::CreateMetaFile() const
//{
//    wxPoint pos = m_shape->GetPosition();
//    wxSize size = m_shape->GetSize();
//
//    wxMetaFileDC dcMF(wxEmptyString, pos.x + size.x, pos.y + size.y);
//
//    m_shape->Draw(dcMF);
//
//    wxMetafile *mf = dcMF.Close();
//
//    DnDShapeDataObject *self = (DnDShapeDataObject *)this; // const_cast
//    self->m_dobjMetaFile.SetMetafile(*mf);
//    self->m_hasMetaFile = true;
//
//    delete mf;
//}
//
//#endif // wxUSE_METAFILE
//
//void wxGxDataObject::CreateBitmap() const
//{
//    wxPoint pos = m_shape->GetPosition();
//    wxSize size = m_shape->GetSize();
//    int x = pos.x + size.x,
//        y = pos.y + size.y;
//    wxBitmap bitmap(x, y);
//    wxMemoryDC dc;
//    dc.SelectObject(bitmap);
//    dc.SetBrush(wxBrush(wxT("white"), wxSOLID));
//    dc.Clear();
//    m_shape->Draw(dc);
//    dc.SelectObject(wxNullBitmap);
//
//    DnDShapeDataObject *self = (DnDShapeDataObject *)this; // const_cast
//    self->m_dobjBitmap.SetBitmap(bitmap);
//    self->m_hasBitmap = true;
//}
//
//#endif // wxUSE_DRAG_AND_DROP