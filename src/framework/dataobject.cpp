/******************************************************************************
 * Project:  wxGIS (desktop)
 * Purpose:  A DataObject represents data that can be copied to or from the clipboard, or dragged and dropped.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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
#include "wxgis/framework/dataobject.h"

#ifdef wxUSE_DRAG_AND_DROP

#define BUF2VALMEMCOPY(val, buf, val_size) memcpy(val, buf, val_size); \
        buf += val_size
#define VAL2BUFMEMCOPY(buf, val, val_size) memcpy(buf, val, val_size); \
        buf += val_size
// ------------------------------------------------------------------------------
// wxGISStringDataObject
// ------------------------------------------------------------------------------

wxGISStringDataObject::wxGISStringDataObject(const wxDataFormat &format) : wxDataObjectSimple(format)
{
}


bool wxGISStringDataObject::SetData(size_t len, const void *buf)
{
    m_oaStrings.Empty();

    unsigned char * pbuf = (unsigned char*)buf;
    size_t nCount(0);

    BUF2VALMEMCOPY(&nCount, pbuf, sizeOfsize_t);

    size_t nStrLen(0), nStrSize(0);
    for (size_t n = 0; n < nCount; ++n )
    {
        BUF2VALMEMCOPY(&nStrLen, pbuf, sizeOfsize_t);

        wxString str((const wxChar*) pbuf, nStrLen);
        m_oaStrings.Add(str);

        nStrSize = nStrLen * sizeOfChar;
        pbuf += nStrSize;
    }
    return true;
}

void wxGISStringDataObject::AddString(const wxString& sStr)
{
    m_oaStrings.Add(sStr);
}

size_t wxGISStringDataObject::GetDataSize() const
{
    if ( m_oaStrings.IsEmpty() )
        return 0;

    size_t sz = sizeOfsize_t;

    for ( size_t i = 0; i < m_oaStrings.GetCount(); ++i )
    {
        sz += sizeOfsize_t + m_oaStrings[i].Len() * sizeOfChar;
    }

    return sz;
}

bool wxGISStringDataObject::GetDataHere(void *pData) const
{
    if ( !pData || m_oaStrings.IsEmpty() )
        return false;

    unsigned char * pbuf = (unsigned char*)pData;

    const size_t nCount = m_oaStrings.GetCount();
    VAL2BUFMEMCOPY(pbuf, &nCount, sizeOfsize_t);

    for ( size_t i = 0; i < nCount; ++i )
    {
        // copy filename to pbuf and add null terminator
        size_t nStrLen = m_oaStrings[i].Len();
        size_t nStrSize = nStrLen * sizeOfChar;
        VAL2BUFMEMCOPY(pbuf, &nStrLen, sizeOfsize_t);

        VAL2BUFMEMCOPY(pbuf, m_oaStrings[i].wc_str(), nStrSize);//t_str
    }

    return true;
}

// ------------------------------------------------------------------------------
// wxGISDecimalDataObject
// ------------------------------------------------------------------------------

wxGISDecimalDataObject::wxGISDecimalDataObject(const wxDataFormat &format) : wxDataObjectSimple(format)
{
}


bool wxGISDecimalDataObject::SetData(size_t len, const void *buf)
{
    m_oaDecimals.Empty();

    unsigned char * pbuf = (unsigned char*)buf;
    size_t nCount(0);

    BUF2VALMEMCOPY(&nCount, pbuf, sizeOfsize_t);

    for (size_t n = 0; n < nCount; ++n )
    {
        long nValue;
        BUF2VALMEMCOPY(&nValue, pbuf, sizeOfLong);

        m_oaDecimals.Add(nValue);
    }
    return true;
}

void wxGISDecimalDataObject::AddDecimal(long nValue)
{
    m_oaDecimals.Add(nValue);
}

size_t wxGISDecimalDataObject::GetDataSize() const
{
    if ( m_oaDecimals.IsEmpty() )
        return 0;

    size_t sz = sizeOfsize_t + m_oaDecimals.GetCount() * sizeOfLong;
    return sz;
}

bool wxGISDecimalDataObject::GetDataHere(void *pData) const
{
    if ( !pData || m_oaDecimals.IsEmpty() )
        return false;

    unsigned char * pbuf = (unsigned char*)pData;

    const size_t nCount = m_oaDecimals.GetCount();

    VAL2BUFMEMCOPY(pbuf, &nCount, sizeOfsize_t);

    for ( size_t i = 0; i < nCount; ++i )
    {
        VAL2BUFMEMCOPY(pbuf, &m_oaDecimals[i], sizeOfLong);
    }

    return true;
}

// ------------------------------------------------------------------------------
// wxGISTaskDataObject
// ------------------------------------------------------------------------------

wxGISTaskDataObject::wxGISTaskDataObject(long nParentPointer, const wxDataFormat &format) : wxGISDecimalDataObject(format)
{
    m_nParentPointer = nParentPointer;
}

bool wxGISTaskDataObject::SetData(size_t len, const void *buf)
{
    m_oaDecimals.Empty();
    m_nParentPointer = wxNOT_FOUND;

    unsigned char * pbuf = (unsigned char*)buf;

    size_t nStrLen(0);
    BUF2VALMEMCOPY(&m_nParentPointer, pbuf, sizeOfLong);

    size_t nCount(0);

    BUF2VALMEMCOPY(&nCount, pbuf, sizeOfsize_t);

    for (size_t n = 0; n < nCount; ++n )
    {
        long nValue;
        BUF2VALMEMCOPY(&nValue, pbuf, sizeOfLong);

        m_oaDecimals.Add(nValue);
    }
    return true;
}

size_t wxGISTaskDataObject::GetDataSize() const
{
    if ( m_oaDecimals.IsEmpty() )
        return 0;

    size_t sz = sizeOfsize_t + m_oaDecimals.GetCount() * sizeOfLong + sizeOfLong;

    return sz;
}

bool wxGISTaskDataObject::GetDataHere(void *pData) const
{
    if ( !pData || m_oaDecimals.IsEmpty() )
        return false;

    unsigned char * pbuf = (unsigned char*)pData;

    VAL2BUFMEMCOPY(pbuf, &m_nParentPointer, sizeOfLong);

    const size_t nCount = m_oaDecimals.GetCount();

    VAL2BUFMEMCOPY(pbuf, &nCount, sizeOfsize_t);

    for ( size_t i = 0; i < nCount; ++i )
    {
        VAL2BUFMEMCOPY(pbuf, &m_oaDecimals[i], sizeOfLong);
    }

    return true;
}


#endif // wxUSE_DRAG_AND_DROP
