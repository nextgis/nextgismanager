/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  display header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2014 Dmitry Baryshnikov
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

#include "wxgis/datasource/datasource.h"

#include "wx/image.h"

#undef LT_OBJDIR
#include "ogrsf_frmts.h"

#ifndef wxGIS_USE_CAIRO
    #error Cairo required
#endif

#define MINPOLYAREA 0.25
#define MINPOLYDRAWAREA 2.2
#define ZOOM_FACTOR 0.4

#define ENVMAX_X 179
#define ENVMAX_Y 85
#define ENVMIN_X -179
#define ENVMIN_Y -85

/** @enum wxGISEnumDrawPhase

    A draw phase

    @library{display}
*/
enum wxGISEnumDrawPhase
{
	wxGISDPGeography = 0x0001,
	wxGISDPAnnotation = 0x0002,
	wxGISDPSelection = 0x0004
};

/** @enum wxGISEnumDrawQuality

    A raster interpolation quality

    @library{display}
*/
enum wxGISEnumDrawQuality
{
	enumGISQualityNearest,
	enumGISQualityBilinear,
	enumGISQualityBicubic,
	enumGISQualityHalfBilinear,
    enumGISQualityHalfQuadBilinear,
    enumGISQualityFourQuadBilinear
};

/** @enum wxGISEnumPointPosition

    A point position enumerator. This enumerator used in Sutherland-Hodgman Polygon Clipping

    @library{display}
*/
enum wxGISEnumPointPosition
{
	enumGISPtPosLeft = 0,
	enumGISPtPosRight,
	enumGISPtPosBottom,
	enumGISPtPosTop
};

/** @enum wxGISEnumDrawStyle

    A styles to be get o set

    @library{display}
enum wxGISEnumDrawStyle
{
	enumGISDrawStyleNone		= 0,
	enumGISDrawStyleFill,
	enumGISDrawStyleOutline,
	enumGISDrawStylePoint
};*/

//class IDisplayTransformation
//{
//public:
//	virtual ~IDisplayTransformation(void){};
//	virtual void Reset(void) = 0;
//	virtual void SetDeviceFrame(wxRect rc) = 0;
//	virtual wxRect GetDeviceFrame(void) = 0;
//	virtual bool IsBoundsSet(void) = 0;
//	virtual void SetBounds(OGREnvelope bounds) = 0;
//	virtual OGREnvelope GetBounds(void) = 0;
//	virtual OGREnvelope GetVisibleBounds(void) = 0;
//	virtual double GetRatio(void) = 0;
//	virtual double GetScaleRatio(void) = 0;
//	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference) = 0;
//	virtual OGRSpatialReference* GetSpatialReference(void) = 0;
//	virtual wxPoint* TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount) = 0;
//	virtual void TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount, wxPoint* pResult) = 0;
//	virtual OGRRawPoint* TransformCoordDC2World(wxPoint* pPoints, size_t nPointCount) = 0;
//	virtual void SetPPI(wxSize new_res) = 0;
//	virtual OGREnvelope TransformRect(wxRect rect) = 0;
//};
//
//typedef std::vector<wxRect> RECTARARRAY;
//
//class IDisplay
//{
//public:
//	virtual ~IDisplay(void){};
//	virtual void OnDraw(wxDC &dc, wxCoord x = 0, wxCoord y = 0, bool bClearBackground = false) = 0;
//    virtual void OnUpdate(void) = 0;
//    virtual void SetDC(wxDC *pdc) = 0;
//	virtual bool IsDerty(void) = 0;
//	virtual void SetDerty(bool bIsDerty) = 0;
//	virtual void SetBrush(wxBrush& Brush) = 0;
//	virtual void SetPen(wxPen& Pen) = 0;
//	virtual void SetFont(wxFont& Font) = 0;
//	virtual void DrawPolygon(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE) = 0;
//	virtual void DrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE) = 0;
//	virtual void DrawPoint(wxCoord x, wxCoord y) = 0;
//    virtual void DrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0) = 0;
//	virtual void DrawCircle(wxCoord x, wxCoord y, wxCoord radius) = 0;
//	virtual void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) = 0;
//	virtual void DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent = false) = 0;
//	virtual IDisplayTransformation* GetDisplayTransformation(void) = 0;
//    virtual RECTARARRAY* GetInvalidRect(void) = 0;
//    virtual void AddInvalidRect(wxRect Rect) = 0;
//};
//
//class ICachedDisplay :
//	public IDisplay
//{
//public:
//	virtual ~ICachedDisplay(void){};
//	virtual void OnStretchDraw(wxDC &dc, wxCoord nDestWidth, wxCoord nDestHeight, wxCoord x = 0, wxCoord y = 0, bool bClearBackground = false, wxGISEnumDrawQuality quality = enumGISQualityNearest) = 0;
//	virtual void OnStretchDraw2(wxDC &dc, wxRect Rect, bool bClearBackground = false, wxGISEnumDrawQuality quality = enumGISQualityNearest) = 0;
//	virtual void OnPanDraw(wxDC &dc, wxCoord x, wxCoord y) = 0;
//    virtual void OnPanStop(wxDC &dc) = 0;
//	//virtual void OnDrawRectangle(wxDC &dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height) = 0;
//	//Check if cache_id is derty
//	virtual bool IsCacheDerty(size_t cache_id) = 0;
//	virtual void SetCacheDerty(size_t cache_id, bool bIsDerty) = 0;
//	virtual size_t AddCache(void) = 0;
//	virtual size_t GetLastCacheID(void) = 0;
//	virtual void MergeCaches(size_t SrcCacheID, size_t DstCacheID) = 0;
//	virtual void StartDrawing(size_t CacheID) = 0;
//	virtual void FinishDrawing(void) = 0;
//	virtual void ClearCaches(void) = 0;
//};
//
//class ISymbol
//{
//public:
////	ISymbol(void){};
//	virtual ~ISymbol(void){};
//	virtual void Draw(OGRGeometry* pGeometry, IDisplay* pDisplay) = 0;
//	virtual void SetPen(wxPen NewPen){m_Pen = NewPen;};
//	virtual wxPen GetPen(void){return m_Pen;};
//	virtual void SetBrush(wxBrush NewBrush){m_Brush = NewBrush;};
//	virtual wxBrush GetBrush(void){return m_Brush;};
//	virtual void SetFont(wxFont NewFont){m_Font = NewFont;};
//	virtual wxFont GetFont(void){return m_Font;};
//protected:
//	wxPen m_Pen;//wxNullPen
//	wxBrush m_Brush;//wxNullBrush
//	wxFont m_Font;//wxNullFont
//};
