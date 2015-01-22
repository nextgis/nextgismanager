/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  icons classes
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2009,2012,2014 Dmitry Baryshnikov
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
#include "wxgis/framework/icon.h"

#include "../../art/lnk_16.xpm"
#include "../../art/lnk_48.xpm"
#include "../../art/wrn_16.xpm"
#include "../../art/wrn_48.xpm"
#include "../../art/err_16.xpm"
#include "../../art/err_48.xpm"
#include "../../art/crt_16.xpm"
#include "../../art/crt_48.xpm"

wxIcon GetStateIcon(const wxIcon & icon, wxGISEnumIconStateType eType, bool bIsLarge)
{
    wxIcon ret;
    switch (eType)
    {
    case wxGISEnumIconStateDisconnect:
    {
        wxImage image = wxBitmap(icon).ConvertToImage();
        if (bIsLarge)
            image.Paste(wxBitmap(err_48_xpm).ConvertToImage(), 32, 32);
        else
            image.Paste(wxBitmap(err_16_xpm).ConvertToImage(), 9, 9);

        ret.CopyFromBitmap(wxBitmap(image));
        break;
    }
    case wxGISEnumIconStateDisconnectAlt:
    {
        wxImage image = wxBitmap(icon).ConvertToImage();
        if (bIsLarge)
            image.Paste(wxBitmap(err_48_xpm).ConvertToImage(), 32, 0);
        else
            image.Paste(wxBitmap(err_16_xpm).ConvertToImage(), 9, 0);

        ret.CopyFromBitmap(wxBitmap(image));
        break;
    }    
    case wxGISEnumIconStateLink:
    {
        wxImage image = wxBitmap(icon).ConvertToImage();
        if (bIsLarge)
            image.Paste(wxBitmap(lnk_48_xpm).ConvertToImage(), 32, 32);
        else
            image.Paste(wxBitmap(lnk_16_xpm).ConvertToImage(), 7, 7);

        ret.CopyFromBitmap(wxBitmap(image));
        break;
    }
    case wxGISEnumIconStateNew:
    {                
        wxImage image = wxBitmap(icon).ConvertToImage();
        if (bIsLarge)
        {
            wxImage fore = wxBitmap(crt_48_xpm).ConvertToImage();
            fore.SetMaskColour(fore.GetRed(0, 0), fore.GetGreen(0, 0), fore.GetBlue(0, 0));
            //fore.InitAlpha();
            image = OverlayImage(image, fore, 32, 0);
        }
        else
        {
            wxImage fore = wxBitmap(crt_16_xpm).ConvertToImage();
            fore.SetMaskColour(fore.GetRed(0, 0), fore.GetGreen(0, 0), fore.GetBlue(0, 0));
            //fore.InitAlpha();
            image = OverlayImage(image, fore, 7, 0);  
        }
        ret.CopyFromBitmap(wxBitmap(image));
        
        break;
    }
    case wxGISEnumIconStateWarning:
    {
        wxImage image = wxBitmap(icon).ConvertToImage();
        if (bIsLarge)
        {
            wxImage fore = wxBitmap(wrn_48_xpm).ConvertToImage();
            fore.SetMaskColour(fore.GetRed(0, 0), fore.GetGreen(0, 0), fore.GetBlue(0, 0));
            //fore.InitAlpha();
            image = OverlayImage(image, fore, 32, 32);
        }
        else
        {
            wxImage fore = wxBitmap(wrn_16_xpm).ConvertToImage();
            fore.SetMaskColour(fore.GetRed(0, 0), fore.GetGreen(0, 0), fore.GetBlue(0, 0));
            //fore.InitAlpha();
            image = OverlayImage(image, fore, 7, 7);
        }
        ret.CopyFromBitmap(wxBitmap(image));

        break;
    }
    };

    return ret;
}

wxImage OverlayImage(const wxImage &imgBack, const wxImage &imgFore, int xoff, int yoff)
{
    wxImage out = imgBack;
    int bgWidth = imgBack.GetWidth();
    int bgHeight = imgBack.GetHeight();
    int fgWidth = imgFore.GetWidth();
    int fgHeight = imgFore.GetHeight();

    if (bgWidth - xoff < fgWidth)
        fgWidth = bgWidth - xoff;
    if (bgHeight - yoff < fgHeight)
        fgHeight = bgHeight - yoff;

    for (int i = 0; i < fgWidth; ++i)
    {
        for (int j = 0; j < fgHeight; ++j)
        {
            int ibk = i + xoff;
            int jbk = j + yoff;
            if (!imgFore.IsTransparent(i, j))
            {
                out.SetRGB(ibk, jbk, imgFore.GetRed(i, j), imgFore.GetGreen(i, j), imgFore.GetBlue(i, j));
            }
        }
    }
    return out;

}