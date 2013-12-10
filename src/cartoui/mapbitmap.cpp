/******************************************************************************
* Project:  wxGIS
* Purpose:  wxGISMapBitmap class.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2013 Bishop
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
#include "wxgis/cartoui/mapbitmap.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/display/displayop.h"
#include "wxgis/catalog/gxfilters.h"

#define UNITS_IN_INCH 2.54

//-----------------------------------------------
// wxGISMapView
//-----------------------------------------------

wxGISMapBitmap::wxGISMapBitmap(int nWidth, int nHeight) : wxGISExtentStack()
{
    m_pGISDisplay = new wxGISDisplay();
	m_pTrackCancel = NULL;
	m_nFactor = 0;

    m_nWidth = nWidth;
    m_nHeight = nHeight;
    wxRect rc(0, 0, nWidth, nHeight);
    m_pGISDisplay->SetDeviceFrame(rc);
}

wxGISMapBitmap::~wxGISMapBitmap(void)
{
    wxDELETE(m_pGISDisplay);
}

void wxGISMapBitmap::SetTrackCancel(ITrackCancel* pTrackCancel)
{
	if(m_pTrackCancel)
		return;
	m_pTrackCancel = pTrackCancel;
	m_pTrackCancel->Reset();
}

bool wxGISMapBitmap::SaveAsBitmap(const CPLString &szPath, wxGISEnumRasterDatasetType eType, char **papszOptions)
{
	if(m_pTrackCancel)
		m_pTrackCancel->Reset();
    if(!m_pGISDisplay)
        return false;

	for(size_t i = 0; i < m_paLayers.size(); ++i)
	{
		if(m_pTrackCancel && !m_pTrackCancel->Continue())
			break;
		wxGISLayer* pLayer = m_paLayers[i];
   		if(!pLayer)
			continue; //not layer

		if(!pLayer->GetVisible())
			continue; //not visible
		if(m_pGISDisplay->IsCacheDerty(pLayer->GetCacheID()))
		{
			//erase bk if layer no 0
			if(i == 0)
				m_pGISDisplay->OnEraseBackground();

			if(m_pGISDisplay->GetDrawCache() != pLayer->GetCacheID())
            {
				m_pGISDisplay->SetDrawCache(pLayer->GetCacheID());
            }

            if (pLayer->Draw(wxGISDPGeography, m_pTrackCancel))
            {
                m_pGISDisplay->SetCacheDerty(pLayer->GetCacheID(), false);
            }
		}
	}
    //wxGxRasterDatasetFilter filter(eType);
    GDALDataset *poDstDS;
    char *pszSRS_WKT = NULL;
    double adfGeoTransform[6] = { 0, 1, 0, 0, 0, 1 };

    GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName("");//filter.GetDriver().mb_str()
    if (poDriver == NULL)
        return false;
    poDstDS = poDriver->Create(szPath, m_nWidth, m_nHeight, 3, GDT_Byte, papszOptions);

    m_SpatialReference->exportToWkt(&pszSRS_WKT);
    poDstDS->SetProjection(pszSRS_WKT);
    CPLFree(pszSRS_WKT);

    double dfX(0), dfY(m_nHeight);
    m_pGISDisplay->DC2World(&dfX, &dfY);
    adfGeoTransform[0] = dfX;
    adfGeoTransform[3] = dfY;

    double dfW(1), dfH(1);
    m_pGISDisplay->DC2WorldDist(&dfW, &dfH);
    adfGeoTransform[1] = dfW;
    adfGeoTransform[5] = dfH;

    poDstDS->SetGeoTransform(adfGeoTransform);

    GDALClose((GDALDatasetH)poDstDS);

}

void wxGISMapBitmap::SetSpatialReference(const wxGISSpatialReference &SpatialReference)
{
	wxGISExtentStack::SetSpatialReference(SpatialReference);
}

bool wxGISMapBitmap::AddLayer(wxGISLayer* pLayer)
{
    wxCHECK_MSG(pLayer, false, wxT("The layer pointer is NULL"));
	//Create cache if needed
	if(pLayer->IsCacheNeeded())
		pLayer->SetCacheID(m_pGISDisplay->AddCache());
	else
		pLayer->SetCacheID(m_pGISDisplay->GetLastCacheID());

	return wxGISExtentStack::AddLayer(pLayer);
}

void wxGISMapBitmap::Clear(void)
{
	m_pGISDisplay->Clear();
	wxGISExtentStack::Clear();
}

void wxGISMapBitmap::SetExtent(const OGREnvelope& Env)
{
	wxGISExtentStack::SetExtent(Env);

	m_pGISDisplay->SetBounds(Env);
}

void wxGISMapBitmap::SetFullExtent(void)
{
	Do(GetFullExtent());
}

void wxGISMapBitmap::SetRotate(double dAngleRad)
{
	m_pGISDisplay->SetRotate(dAngleRad);
    m_dfCurrentAngleRad = DOUBLEPI - dAngleRad;
    if (m_dfCurrentAngleRad >= DOUBLEPI)
        m_dfCurrentAngleRad -= DOUBLEPI;
    if (m_dfCurrentAngleRad < 0)
        m_dfCurrentAngleRad += DOUBLEPI;
}

double wxGISMapBitmap::GetRotate(void) const 
{
    return m_dfCurrentAngleRad;
}

OGREnvelope wxGISMapBitmap::GetFullExtent(void)
{
	OGREnvelope OutputEnv;
    if (!IsDoubleEquil(m_dfCurrentAngleRad, 0.0))
	{
		OutputEnv = m_FullExtent;
		double dCenterX = m_FullExtent.MinX + (m_FullExtent.MaxX - m_FullExtent.MinX) / 2;
		double dCenterY = m_FullExtent.MinY + (m_FullExtent.MaxY - m_FullExtent.MinY) / 2;
        RotateEnvelope(OutputEnv, m_dfCurrentAngleRad, dCenterX, dCenterY);
		IncreaseEnvelope(OutputEnv, 0.1);
	}
    else
    {
		OutputEnv = wxGISMap::GetFullExtent();
    }
	return OutputEnv;
}

void wxGISMapBitmap::DrawGeometry(const wxGISGeometry &Geometry, wxGISSymbol* const pSymbol)
{
    wxCHECK_RET(Geometry.IsOk() && pSymbol, wxT("Input geometry or symbol is not valid"));

    pSymbol->SetupDisplay(m_pGISDisplay);
    pSymbol->Draw(Geometry);
 //   m_pGISDisplay->SetSymbol( pSymbol );
	//m_pGISDisplay->DrawGeometry( Geometry );
}
