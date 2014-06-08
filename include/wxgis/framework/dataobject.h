/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  A DataObject represents data that can be copied to or from the clipboard, or dragged and dropped.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Dmitry Barishnikov
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

#include "wxgis/framework/framework.h"

#ifdef wxUSE_DRAG_AND_DROP

/** @class wxGISStringDataObject

    A wxGISStringDataObject represents string data that can be copied to or from the clipboard, or dragged and dropped.

    @library{framework}
*/

class WXDLLIMPEXP_GIS_FRW wxGISStringDataObject : public wxDataObjectSimple
{
public:
    // ctor: use AddFile() later to fill the array
    wxGISStringDataObject(const wxDataFormat &format = wxFormatInvalid);

    // get a reference to our array
    const wxArrayString& GetStrings() const { return m_oaStrings; }
    // implement base class pure virtuals
    virtual bool SetData(size_t len, const void *buf);
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *pData) const;
    virtual void AddString(const wxString& sStr);

protected:
    wxArrayString m_oaStrings;

    static const size_t sizeOfChar = sizeof(wxChar);
    static const size_t sizeOfsize_t = sizeof(size_t);

private:
    wxDECLARE_NO_COPY_CLASS(wxGISStringDataObject);
};

/** @class wxGISDecimalDataObject

    A wxGISDecimalDataObject represents decimal data that can be copied to or from the clipboard, or dragged and dropped.

    @library{framework}
*/

class WXDLLIMPEXP_GIS_FRW wxGISDecimalDataObject : public wxDataObjectSimple
{
public:
    // ctor: use AddFile() later to fill the array
    wxGISDecimalDataObject(const wxDataFormat &format = wxFormatInvalid);

    // get a reference to our array
    const wxArrayLong& GetDecimals() const { return m_oaDecimals; }
    // implement base class pure virtuals
    virtual bool SetData(size_t len, const void *buf);
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *pData) const;
    //
    virtual void AddDecimal(long nValue);

protected:
    wxArrayLong m_oaDecimals;

    static const size_t sizeOfLong = sizeof(long);
    static const size_t sizeOfsize_t = sizeof(size_t);

private:
    wxDECLARE_NO_COPY_CLASS(wxGISDecimalDataObject);
};


/** @class wxGISDecimalDataObject

    A wxGISDecimalDataObject represents decimal data that can be copied to or from the clipboard, or dragged and dropped.

    @library{framework}
*/

class WXDLLIMPEXP_GIS_FRW wxGISTaskDataObject : public wxGISDecimalDataObject
{
public:
    // ctor: use AddFile() later to fill the array
    wxGISTaskDataObject(long nParentPointer, const wxDataFormat &format = wxFormatInvalid);

    virtual bool SetData(size_t len, const void *buf);
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *pData) const;
    //
    const long GetParentPointer() const { return m_nParentPointer; }


protected:
    long m_nParentPointer;//window id for prevent cross app dnd

private:
    wxDECLARE_NO_COPY_CLASS(wxGISTaskDataObject);
};

#endif // wxUSE_DRAG_AND_DROP
