/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Drag'n'Drop wxGIS implementations.
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
#pragma once

#include "wxgis/framework/framework.h"

#define DNDSCROLL 10 

/** \class wxGISDropTarget droptarget.h
    \brief A DropTarget for catalog items.
*/

class WXDLLIMPEXP_GIS_FRW wxGISDropTarget : public wxFileDropTarget
{
public:
    wxGISDropTarget(IViewDropTarget *pOwner);
    virtual ~wxGISDropTarget(void);
    //wxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    virtual void OnLeave();
    //wxFileDropTarget
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
    IViewDropTarget *m_pOwner;
};


//static const wxChar *shapeFormatId = wxT("wxShape");

/** \class wxGxDataObject gxcatdroptarget.h
    \brief A DropTarget for catalog items.
*/

//class wxGxDataObject : public wxFileDataObject
//{
//public:
//    // ctor doesn't copy the pointer, so it shouldn't go away while this object
//    // is alive
//    wxGxDataObject(int nType, int nSubType);
//    virtual ~wxGxDataObject();
//    DnDShape *GetShape();
//    virtual wxDataFormat GetPreferredFormat(Direction WXUNUSED(dir)) const;
//    virtual size_t GetFormatCount(Direction dir) const;
//    virtual void GetAllFormats(wxDataFormat *formats, Direction dir) const;
//    virtual size_t GetDataSize(const wxDataFormat& format) const;
//    virtual bool GetDataHere(const wxDataFormat& format, void *pBuf) const;
//    virtual bool SetData(const wxDataFormat& format, size_t WXUNUSED(len), const void *buf);
//private:
//    // creates a bitmap and assigns it to m_dobjBitmap (also sets m_hasBitmap)
//    void CreateBitmap() const;
//#if wxUSE_METAFILE
//    void CreateMetaFile() const;
//#endif // wxUSE_METAFILE
//    wxDataFormat        m_formatShape;  // our custom format
//    wxBitmapDataObject  m_dobjBitmap;   // it handles bitmaps
//    bool                m_hasBitmap;    // true if m_dobjBitmap has valid bitmap
//#if wxUSE_METAFILE
//    wxMetaFileDataObject m_dobjMetaFile;// handles metafiles
//    bool                 m_hasMetaFile; // true if we have valid metafile
//#endif // wxUSE_METAFILE
//    DnDShape           *m_shape;        // our data
//protected:
//    int m_nType;
//    int m_nSubType;
//};

