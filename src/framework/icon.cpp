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

    for (int i = xoff; i < bgWidth; ++i)
    {
        for (int j = yoff; j < fgHeight; ++j)
        {
            int ifore = i - xoff;
            int jfore = j - yoff;
            if (!imgFore.IsTransparent(ifore, jfore))
                out.SetRGB(i, j, imgFore.GetRed(ifore, jfore), imgFore.GetGreen(ifore, jfore), imgFore.GetBlue(ifore, jfore));
        }
    }
    return out;

    /*
    if (!imgFore.HasAlpha())
        return imgBack;

    wxASSERT(imgFore.HasAlpha());

    unsigned char *bg = imgBack.GetData();
    unsigned char *mkbg = imgBack.GetAlpha();
    unsigned char *fg = imgFore.GetData();
    unsigned char *mk = imgFore.GetAlpha();

    int bgWidth = imgBack.GetWidth();
    int bgHeight = imgBack.GetHeight();
    int fgWidth = imgFore.GetWidth();
    int fgHeight = imgFore.GetHeight();


    //Now, determine the dimensions of the images to be masked together
    //on top of the background.  This should be equal to the area of the
    //smaller of the foreground and the mask, as long as it is
    //within the area of the background, given the offset.

    //Make sure the foreground size is no bigger than the mask
    int wCutoff = fgWidth;
    int hCutoff = fgHeight;


    // If the masked foreground + offset is bigger than the background, masking
    // should only occur within these bounds of the foreground image
    wCutoff = (bgWidth - xoff > wCutoff) ? wCutoff : bgWidth - xoff;
    hCutoff = (bgHeight - yoff > hCutoff) ? hCutoff : bgHeight - yoff;

    //Make a new image the size of the background
    wxImage dstImage(bgWidth, bgHeight);
    unsigned char *dst = dstImage.GetData();
    memcpy(dst, bg, bgWidth * bgHeight * 3);
    
    // Go through the foreground image bit by bit and mask it on to the
    // background, at an offset of xoff,yoff.
    // BUT...Don't go beyond the size of the background image,
    // the foreground image, or the mask
    int x, y;
    for (y = 0; y < hCutoff; y++) {

        unsigned char *bkp = bg + 3 * ((y + yoff) * bgWidth + xoff);
        unsigned char *dstp = dst + 3 * ((y + yoff) * bgWidth + xoff);

        for (x = 0; x < wCutoff; x++) {

            int value = mk[(y * fgWidth + x)];// Don't multiply by 3...
            int opp = 255 - value;

            for (int c = 0; c < 3; c++)
                dstp[x * 3 + c] =
                ((bkp[x * 3 + c] * opp) +
                (fg[3 * (y * fgWidth + x) + c] * value)) / 255;
        }
    }
    return dstImage;*/

}