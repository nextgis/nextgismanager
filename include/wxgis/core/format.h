/******************************************************************************
 * Project:  wxGIS
 * Purpose:  format clases (for format coordinates etc.).
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/core/core.h"
#include "wxgis/core/json/jsonval.h"

#include <wx/xml/xml.h>

/** \fn wxString DoubleToString(double Val, bool IsLon) format.h
    \brief Format double value as string coordinate
*/
WXDLLIMPEXP_GIS_CORE wxString DoubleToString(double dVal, bool bIsLon);
/** \fn static double StringToDouble(wxString Val, wxString asterisk) format.h
    \brief Format string coordinate as double value 
*/
WXDLLIMPEXP_GIS_CORE double StringToDouble(const wxString &sVal, const wxString &sAsterisk);

WXDLLIMPEXP_GIS_CORE void SetFloatValue(wxXmlNode* pNode, const wxString &sAttrName, double dfVal);
WXDLLIMPEXP_GIS_CORE void SetFloatValue(wxXmlNode* pNode, const wxString &sAttrName, float dfVal);
WXDLLIMPEXP_GIS_CORE double GetFloatValue(const wxXmlNode* pNode, const wxString &sAttrName, double dfDefVal);
WXDLLIMPEXP_GIS_CORE float GetFloatValue(const wxXmlNode* pNode, const wxString &sAttrName, float dfDefVal);
WXDLLIMPEXP_GIS_CORE void SetDateValue(wxXmlNode* pNode, const wxString &sAttrName, const wxDateTime &dtVal);
WXDLLIMPEXP_GIS_CORE wxJSONValue SetDateValue(const wxDateTime &dtVal);
WXDLLIMPEXP_GIS_CORE wxDateTime GetDateValue(const wxXmlNode* pNode, const wxString &sAttrName, const wxDateTime &dtDefVal);
WXDLLIMPEXP_GIS_CORE wxDateTime GetDateValue(const wxJSONValue &val, const wxString &sAttrName, const wxDateTime &dtDefVal);
WXDLLIMPEXP_GIS_CORE void SetBoolValue(wxXmlNode* pNode, const wxString &sAttrName, bool bVal);
WXDLLIMPEXP_GIS_CORE bool GetBoolValue(const wxXmlNode* pNode, const wxString &sAttrName, bool bDefVal);
WXDLLIMPEXP_GIS_CORE void SetDecimalValue(wxXmlNode* pNode, const wxString &sAttrName, long nVal);
WXDLLIMPEXP_GIS_CORE void SetDecimalValue(wxXmlNode* pNode, const wxString &sAttrName, int nVal);
WXDLLIMPEXP_GIS_CORE long GetDecimalValue(const wxXmlNode* pNode, const wxString &sAttrName, long nDefVal);
WXDLLIMPEXP_GIS_CORE int GetDecimalValue(const wxXmlNode* pNode, const wxString &sAttrName, int nDefVal);

WXDLLIMPEXP_GIS_CORE void FloatStringToCLoc(wxString & str);
WXDLLIMPEXP_GIS_CORE void FloatStringFromCLoc(wxString & str);

WXDLLIMPEXP_GIS_CORE int GetDecimalValue(const wxXmlNode* pNode, const wxString &sAttrName, int nDefVal);

WXDLLIMPEXP_GIS_CORE wxString GetSubString(const wxString &sInputString, const wxString &sBegin, const wxString &sEnd, bool bAppendBegin = false);
WXDLLIMPEXP_GIS_CORE wxString DecodeHTMLEntities(const wxString &sSrc);


/** \fn static wxString NumberScale(double fScaleRatio) format.h
    \brief Format map scale text
*/
WXDLLIMPEXP_GIS_CORE wxString NumberScale(double dScaleRatio);

/** \class wxGISCoordinatesFormat format.h
    \brief The class to format coordinates according different masks
*/
class WXDLLIMPEXP_GIS_CORE wxGISCoordinatesFormat
{
public:
    wxGISCoordinatesFormat(void);
    wxGISCoordinatesFormat(const wxString &sMask, bool bSwap = false);
    virtual ~wxGISCoordinatesFormat(void);
	virtual wxString Format(double dX, double dY);
	virtual bool IsOk(void){return m_bIsOk;};
	virtual void Create(const wxString &sMask, bool bSwap = false);
    virtual wxString GetMask() const {return m_sMask;};
public:
	typedef enum coord_format {  
            D,
            DM,
            DMS,
            M,
            MS,
            S
    } COORD_FORMAT;
protected:
	wxString FormatToken(int nValHigh, int nValLow);
	wxString FormatString(double dCoord, const wxString &sFormat, COORD_FORMAT nCoordFormat, bool bLat);
	wxString ParseString(const wxString &sMask, COORD_FORMAT* pCoordFormat);
protected:
	bool m_bIsOk;
    bool m_bSwaped;
    wxString m_sMask;
    wxString m_sFormatX, m_sFormatY, m_sDivider;
    COORD_FORMAT m_CoordFormatX, m_CoordFormatY;
    bool m_bSign;
};

