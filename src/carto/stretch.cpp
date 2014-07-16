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

#include "wxgis/carto/stretch.h"

#include "wxgis/core/config.h"
#include "wxgis/core/app.h"

///////////////////////////////////////////////////////////////////////////////
// wxGISStretch
///////////////////////////////////////////////////////////////////////////////

wxGISStretch::wxGISStretch(double dfMin, double dfMax, double dfMean, double dfStdDev, double dfNoData)
{
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
	    wxString sAppName = GetApplication()->GetAppName();
        m_dfStdDevParam = oConfig.ReadDouble(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/stretch_stddevparam")), 2.0);
        m_eType = (wxGISEnumRasterStretch)oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/stretch_type")), enumGISRasterStretchStdDev);  // enumGISRasterStretchNone;//   
    }
    else
    {
        m_dfStdDevParam = 2.0;
        m_eType = enumGISRasterStretchStdDev;  
    }

    m_dfMin = dfMin;
    m_dfMax = dfMax;
    m_dfMean = dfMean;
    m_dfStdDev = dfStdDev;
    m_dfNoData = dfNoData;
    m_bInvert = false;

    RecalcEquation();
}

wxGISStretch::~wxGISStretch()
{
}

unsigned char wxGISStretch::GetValue(const double *pdfInput)
{
    unsigned char cOutput;
    switch(m_eType)
    {
    case enumGISRasterStretchNone:
        cOutput = (unsigned char)(*pdfInput);
        break;
    case enumGISRasterStretchStdDev:
        {
            double dVal = m_dfM * ((*pdfInput) - m_dfDX);
            if(dVal < 0)
                cOutput = 0;
            else if(dVal > 255)
                cOutput = 255;
            else
                cOutput = dVal;
        }
        break;
    };
    
    if(m_bInvert)
        return 255 - cOutput;
	return cOutput;
}

bool wxGISStretch::IsNoData(const double& cVal)
{
    return IsDoubleEquil(cVal, m_dfNoData);
}

void wxGISStretch::SetNoData(double dfNoData)
{
    m_dfNoData = dfNoData;
}

double wxGISStretch::GetNoData(void)
{
    return m_dfNoData;
}

void wxGISStretch::SetInvert(bool bInvert)
{
    m_bInvert = bInvert;
}

bool wxGISStretch::GetInvert(void)
{
    return m_bInvert;
}

void wxGISStretch::SetStdDevParam(double dfStdDevParam)
{
    m_dfStdDevParam = dfStdDevParam;
    RecalcEquation();
}

double wxGISStretch::GetStdDevParam(void)
{
    return m_dfStdDevParam;
}

void wxGISStretch::SetStats(double dfMin, double dfMax, double dfMean, double dfStdDev)
{
    m_dfMin = dfMin;
    m_dfMax = dfMax;
    m_dfMean = dfMean;
    m_dfStdDev = dfStdDev;
    if(IsDoubleEquil(m_dfStdDev, 0))
        m_dfStdDev = DEFAULT_STDDEV;
    RecalcEquation();
}

void wxGISStretch::RecalcEquation(void)
{
    switch(m_eType)
    {
    case enumGISRasterStretchStdDev:
        {
            double dfStep = m_dfStdDev * m_dfStdDevParam;
            double dfMin = m_dfMean - dfStep;
            double dfMax = m_dfMean + dfStep;
            CalcEquation(dfMin, dfMax);
        }       
        break;
    case enumGISRasterStretchNone:
    default:
        break;
    };
}

void wxGISStretch::CalcEquation(double dfMin, double dfMax)
{
    //if(dfMin < 0) dfMin = 0;
    //if(dfMax > 255) dfMax = 255;
    //y = m(x - dfMin) + 0
    //y = m(x - ax) + ay
    //y = m(x - m_dfDX)
    m_dfM = 255.0 / (dfMax - dfMin);
    m_dfDX = dfMin;
}
