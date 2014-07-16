/******************************************************************************
 * Project:  wxGIS
 * Purpose:  symbols for feature renderer
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013-2014 Dmitry Baryshnikov
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

#include "wxgis/display/gisdisplay.h"
#include "wxgis/display/color.h"
#include "wxgis/datasource/gdalinh.h"
#include "wxgis/core/pointer.h"

/** @class wxGISSymbol

    The interface base class for all symbols

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISSymbol :
    public wxObject,
    public wxGISPointer
{
    DECLARE_CLASS(wxGISSymbol)
public:
    wxGISSymbol();
    wxGISSymbol(const wxGISColor& Color);
    virtual ~wxGISSymbol();
    virtual void SetupDisplay(wxGISDisplay* const pDisplay);
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0) = 0;
    virtual wxGISColor GetColor() const;
    virtual void SetColor(const wxGISColor& Color);
protected:
    wxGISColor m_Color;
    wxGISDisplay* m_pDisplay;
};

/** @enum wxGISEnumLineCup

    The line cup styles

    @library{display}
 */
enum wxGISEnumLineCup
{
	enumGISLineCupSquare,
	enumGISLineCupRound,
	enumGISLineCupButt
};

/** @enum wxGISEnumLineJoin

    The line join styles

    @library{display}
 */
enum wxGISEnumLineJoin
{
	enumGISLineJoinMiter,
	enumGISLineJoinBevel,
	enumGISLineJoinRound
};

/** @class wxGISSimpleLineSymbol

    The class for simple line symbols

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISSimpleLineSymbol : public wxGISSymbol
{
    DECLARE_CLASS(wxGISSimpleLineSymbol)
public:
    wxGISSimpleLineSymbol();
    wxGISSimpleLineSymbol(const wxGISColor& Color, double dfWidth);
    virtual ~wxGISSimpleLineSymbol(void);
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual void SetStyleToDisplay();
    virtual void SetLimitStyleToDisplay();
    virtual bool DrawPreserved(const OGRLineString* pLine, bool bIsRing = false);
    virtual double GetWidth() const;
    virtual void SetWidth(double dfWidth);
    virtual wxGISEnumLineCup GeCup() const;
    virtual void SetCup(wxGISEnumLineCup eCup);
    virtual wxGISEnumLineJoin GetJoin() const;
    virtual void SetJoin(wxGISEnumLineJoin eJoin);
    virtual double GetMiterLimit() const;
    virtual void SetMiterLimit(double dfMiterLimit);
    virtual wxGISSimpleLineSymbol* Clone() const;
protected:
    double m_dfWidth;
    wxGISEnumLineCup m_eCup;
    wxGISEnumLineJoin m_eJoin;
    double m_dfMiterLimit;
};

/** @enum wxGISEnumFillRule

    The line join styles

    @library{display}
 */
enum wxGISEnumFillRule
{
	enumGISFillRuleWinding,
	enumGISFillRuleOdd
};

/** @class wxGISSimpleFillSymbo

    The class for simple fill symbols

    @library{display}
*/
class WXDLLIMPEXP_GIS_DSP wxGISSimpleFillSymbol : public wxGISSymbol
{
    DECLARE_CLASS(wxGISSimpleFillSymbol)
public:
    wxGISSimpleFillSymbol();
    wxGISSimpleFillSymbol(const wxGISColor& Color, wxGISSimpleLineSymbol *pLineSymbol);
    virtual ~wxGISSimpleFillSymbol();
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual void SetupDisplay(wxGISDisplay* const pDisplay);
    virtual wxGISEnumFillRule GetFillRule() const;
    virtual void SetFillRule(wxGISEnumFillRule eFillRule);
    virtual wxGISSimpleLineSymbol *GetSimpleLineSymbol() const;
    virtual void SetSimpleLineSymbol(wxGISSimpleLineSymbol *pLineSymbol);
    virtual wxGISSimpleFillSymbol* Clone() const;
protected:
    wxGISSimpleLineSymbol *m_pLineSymbol;
    wxGISEnumFillRule m_eFillRule;
};

/** @class wxGISSimpleCircleSymbol

    The class for simple circle symbols

    @library{display}
*/
class WXDLLIMPEXP_GIS_DSP wxGISSimpleCircleSymbol : public wxGISSimpleFillSymbol
{
    DECLARE_CLASS(wxGISSimpleCircleSymbol)
public:
    wxGISSimpleCircleSymbol();
    wxGISSimpleCircleSymbol(const wxGISColor& Color, wxGISSimpleLineSymbol *pLineSymbol);
    virtual ~wxGISSimpleCircleSymbol();
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual wxGISSimpleCircleSymbol* Clone() const;
};


/** @class wxGISSimpleEllipseSymbol

    The class for simple ellipse symbols

    @library{display}
*/
class WXDLLIMPEXP_GIS_DSP wxGISSimpleEllipseSymbol : public wxGISSimpleFillSymbol
{
    DECLARE_CLASS(wxGISSimpleEllipseSymbol)
public:
    wxGISSimpleEllipseSymbol();
    wxGISSimpleEllipseSymbol(const wxGISColor& Color, wxGISSimpleLineSymbol *pLineSymbol);
    virtual ~wxGISSimpleEllipseSymbol();
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual wxGISSimpleEllipseSymbol* Clone() const;
};

/** @class wxGISSimpleMarkerSymbol

    The class for simple marker symbols

    @library{display}
*/
class WXDLLIMPEXP_GIS_DSP wxGISSimpleMarkerSymbol : public wxGISSymbol
{
    DECLARE_CLASS(wxGISSimpleMarkerSymbol)
public:
    wxGISSimpleMarkerSymbol();
    wxGISSimpleMarkerSymbol(const wxGISColor& Color, double dfSize);
    virtual ~wxGISSimpleMarkerSymbol();
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual void SetSize(double dfSize);
    virtual double GetSize() const;
    virtual void SetOutlineColor(const wxGISColor &Color);
    virtual wxGISColor GetOutlineColor() const;
    virtual void SetOutlineSize(double dfSize);
    virtual double GetOutlineSize() const;
    virtual wxGISSimpleMarkerSymbol* Clone() const;
protected:
    double m_dfSize, m_dfOutlineSize;
    wxGISColor m_OutlineColor;
};


/** @class wxGISSimpleCollectionSymbol

    The class for simple geometry collecton symbols

    @library{display}
*/
class WXDLLIMPEXP_GIS_DSP wxGISSimpleCollectionSymbol : public wxGISSymbol
{
    DECLARE_CLASS(wxGISSimpleCollectionSymbol)
public:
    wxGISSimpleCollectionSymbol();
    wxGISSimpleCollectionSymbol(const wxGISColor& Color, wxGISSimpleMarkerSymbol* pMarkerSymbol, wxGISSimpleLineSymbol* pLineSymbol, wxGISSimpleFillSymbol* pFillSymbol);
    virtual ~wxGISSimpleCollectionSymbol();
    virtual void Draw(const wxGISGeometry &Geometry, int nLevel = 0);
    virtual void SetupDisplay(wxGISDisplay* const pDisplay);
    virtual wxGISSimpleCollectionSymbol* Clone() const;
    virtual wxGISSimpleMarkerSymbol* GetMarkerSymbol() const;
    virtual wxGISSimpleLineSymbol* GetLineSymbol() const;
    virtual wxGISSimpleFillSymbol* GetFillSymbol() const;
protected:
    wxGISSimpleMarkerSymbol* m_pMarkerSymbol;
    wxGISSimpleLineSymbol* m_pLineSymbol;
    wxGISSimpleFillSymbol* m_pFillSymbol;
};
