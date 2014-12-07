/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxrasterfactoryui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/raster_16.xpm"
#include "../../art/raster_48.xpm"
#include "../../art/raster_bmp16.xpm"
#include "../../art/raster_bmp48.xpm"
#include "../../art/raster_tif16.xpm"
#include "../../art/raster_tif48.xpm"
#include "../../art/raster_unk16.xpm"
#include "../../art/raster_unk48.xpm"
#include "../../art/raster_til16.xpm"
#include "../../art/raster_til48.xpm"
#include "../../art/raster_img16.xpm"
#include "../../art/raster_img48.xpm"
#include "../../art/raster_png16.xpm"
#include "../../art/raster_png48.xpm"
#include "../../art/raster_gif16.xpm"
#include "../../art/raster_gif48.xpm"
#include "../../art/raster_saga16.xpm"
#include "../../art/raster_saga48.xpm"
#include "../../art/raster_vrt16.xpm"
#include "../../art/raster_vrt48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactoryUI, wxGxRasterFactory)

wxGxRasterFactoryUI::wxGxRasterFactoryUI(void) : wxGxRasterFactory()
{
}

wxGxRasterFactoryUI::~wxGxRasterFactoryUI(void)
{
}

wxGxObject* wxGxRasterFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumRasterDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxObject* pRet = NULL;
    wxIcon icLargeIcon, icSmallIcon;
    //different icons for rasters
    switch(type)
    {
    case enumRasterBmp:
        if(!m_icBMPSmallIcon.IsOk())
            m_icBMPSmallIcon = wxIcon(raster_bmp16_xpm);
        if(!m_icBMPLargeIcon.IsOk())
            m_icBMPLargeIcon = wxIcon(raster_bmp48_xpm);
        icLargeIcon = m_icBMPLargeIcon;
        icSmallIcon = m_icBMPSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterTiff:
        if(!m_icTIFSmallIcon.IsOk())
            m_icTIFSmallIcon = wxIcon(raster_tif16_xpm);
        if(!m_icTIFLargeIcon.IsOk())
            m_icTIFLargeIcon = wxIcon(raster_tif48_xpm);
        icLargeIcon = m_icTIFLargeIcon;
        icSmallIcon = m_icTIFSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterTil:
        if(!m_icTILSmallIcon.IsOk())
            m_icTILSmallIcon = wxIcon(raster_til16_xpm);
        if(!m_icTILLargeIcon.IsOk())
            m_icTILLargeIcon = wxIcon(raster_til48_xpm);
        icLargeIcon = m_icTILLargeIcon;
        icSmallIcon = m_icTILSmallIcon;
        {
            wxGxRasterDatasetContainerUI* pDataset = new wxGxRasterDatasetContainerUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterImg:
        if(!m_icIMGSmallIcon.IsOk())
            m_icIMGSmallIcon = wxIcon(raster_img16_xpm);
        if(!m_icIMGLargeIcon.IsOk())
            m_icIMGLargeIcon = wxIcon(raster_img48_xpm);
        icLargeIcon = m_icIMGLargeIcon;
        icSmallIcon = m_icIMGSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterJpeg:
        if(!m_icJPGSmallIcon.IsOk())
            m_icJPGSmallIcon = wxIcon(raster_16_xpm);
        if(!m_icJPGLargeIcon.IsOk())
            m_icJPGLargeIcon = wxIcon(raster_48_xpm);
        icLargeIcon = m_icJPGLargeIcon;
        icSmallIcon = m_icJPGSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterPng:
        if(!m_icPNGSmallIcon.IsOk())
            m_icPNGSmallIcon = wxIcon(raster_png16_xpm);
        if(!m_icPNGLargeIcon.IsOk())
            m_icPNGLargeIcon = wxIcon(raster_png48_xpm);
        icLargeIcon = m_icPNGLargeIcon;
        icSmallIcon = m_icPNGSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterGif:
        if(!m_icGIFSmallIcon.IsOk())
            m_icGIFSmallIcon = wxIcon(raster_gif16_xpm);
        if(!m_icGIFLargeIcon.IsOk())
            m_icGIFLargeIcon = wxIcon(raster_gif48_xpm);
        icLargeIcon = m_icGIFLargeIcon;
        icSmallIcon = m_icGIFSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterSAGA:
        if(!m_icSAGASmallIcon.IsOk())
            m_icSAGASmallIcon = wxIcon(raster_saga16_xpm);
        if(!m_icSAGALargeIcon.IsOk())
            m_icSAGALargeIcon = wxIcon(raster_saga48_xpm);
        icLargeIcon = m_icSAGALargeIcon;
        icSmallIcon = m_icSAGASmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    case enumRasterVRT:
        if(!m_icVRTSmallIcon.IsOk())
            m_icVRTSmallIcon = wxIcon(raster_vrt16_xpm);
        if(!m_icVRTLargeIcon.IsOk())
            m_icVRTLargeIcon = wxIcon(raster_vrt48_xpm);
        icLargeIcon = m_icVRTLargeIcon;
        icSmallIcon = m_icVRTSmallIcon;
        //TODO: container too
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    default:
        if(!m_icSmallIcon.IsOk())
            m_icSmallIcon = wxIcon(raster_unk16_xpm);
        if(!m_icLargeIcon.IsOk())
            m_icLargeIcon = wxIcon(raster_unk48_xpm);
        icLargeIcon = m_icLargeIcon;
        icSmallIcon = m_icSmallIcon;
        {
            wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(type, pParent, soName, szPath, icLargeIcon, icSmallIcon);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    };

    return pRet; 
}

