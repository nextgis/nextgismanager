/******************************************************************************
 * Project:  wxGIS
 * Purpose:  pixel field stretch classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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

#include "wxgis/carto/carto.h"

//TODO:
//esriRasterStretch_NONE	0	Stretch is not applied.
//esriRasterStretch_DefaultFromSource	1	Stretch is default type.
//esriRasterStretch_Custom	2	Stretch is a custom type.
//esriRasterStretch_StandardDeviations	3	Stretch is a standard deviation type.
//esriRasterStretch_HistogramEqualize	4	Stretch is a histogram equalization type.
//esriRasterStretch_MinimumMaximum	5	Stretch is a minimum-maximum type.
//esriRasterStretch_HistogramSpecification	6	Stretch is a Histogram Specification type.
//esriRasterStretch_Count	7	Stretch is a count type.

#define DEFAULT_STDDEV 60.0

/** \enum wxGISEnumRasterStretch
    \brief The raster pixel color stretch types.
*/
enum wxGISEnumRasterStretch
{
	enumGISRasterStretchNone = 0,   /**< No Stretch */
	enumGISRasterStretchStdDev      /**< Stretch use a standard deviation */
};

/** \class wxGISStretch stretch.h
    \brief The base class for stretch from pixel field to display field

	This is more close to compress because input pixels range may be from -100000 to 100000 and output - in 0 - 255 range
*/

class wxGISStretch
{
public:
	wxGISStretch(double dfMin = 0.0, double dfMax = 255.0, double dfMean = 127.5, double dfStdDev = DEFAULT_STDDEV, double dfNoData = NOTNODATA);
	virtual ~wxGISStretch(void);
	virtual unsigned char GetValue(const double *pdfInput);
    virtual bool IsNoData(const double& cVal);
    virtual void SetNoData(double dfNoData);
    virtual double GetNoData(void);
    virtual void SetInvert(bool bInvert);
    virtual bool GetInvert(void);
    virtual void SetStdDevParam(double dfStdDevParam);
    virtual double GetStdDevParam(void);
    virtual void SetStats(double dfMin = 0.0, double dfMax = 255.0, double dfMean = 127.5, double dfStdDev = DEFAULT_STDDEV);
protected:
    virtual void RecalcEquation(void);
    virtual void CalcEquation(double dfMin, double dfMax);
    //AdjastBritnessContrast
protected:
    double m_dfNoData;
	double m_dfMin;
	double m_dfMax;
	double m_dfMean;
	double m_dfStdDev;
    bool m_bInvert;
    double m_dfStdDevParam;
    wxGISEnumRasterStretch m_eType;
protected:
    double m_dfM, m_dfDX;
};

//TODO: esriRasterStretchStatsTypeEnum Constants
//Raster stretch statistics types.
//esriRasterStretchStats_AreaOfView	0	Stretch stats from current Area Of View.
//esriRasterStretchStats_Dataset	1	Stretch stats from current Dataset being rendered.
//esriRasterStretchStats_GlobalStats	2	Stretch stats from global stats definition.

//stretch - none, custom, standard derivations, histogram equalize, min-max, histogram specification, percent clip
//class wxGISStra : public wxGISStretch
//{
//};
