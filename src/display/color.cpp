/******************************************************************************
 * Project:  wxGIS
 * Purpose:  color functions and classes
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
#include "wxgis/display/color.h"

#include "wx/tokenzr.h"

//---------------------------------------------------------------------------
// wxGISColor
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISColor, wxColour)

wxGISColor::wxGISColor() : wxColour()
{
}

wxGISColor::wxGISColor(ChannelType red, ChannelType green, ChannelType blue, ChannelType alpha) : wxColour(red, green, blue, alpha)
{
    SetValues(red, green, blue, alpha);
}

wxGISColor::wxGISColor(const wxGISColor &Color) : wxColour(Color.Red(), Color.Green(), Color.Blue(), Color.Alpha()) 
{
    SetValues(Color.Red(), Color.Green(), Color.Blue(), Color.Alpha());
}
 
wxGISColor::wxGISColor(const wxColour &Color) : wxColour(Color) 
{
    SetValues(Color.Red(), Color.Green(), Color.Blue(), Color.Alpha());
}
 
wxColour wxGISColor::GetColour() const
{
    return wxColour(Red(), Green(), Blue(), Alpha()); 
}

void wxGISColor::Set(ChannelType red, ChannelType green, ChannelType blue, ChannelType alpha)
{
    wxColour::Set(red, green, blue, alpha);
    SetValues(red, green, blue, alpha);
}

void wxGISColor::SetValues(ChannelType red, ChannelType green, ChannelType blue, ChannelType alpha)
{
    m_dfRed = double(red) / 255;
    m_dfBlue = double(blue) / 255;
    m_dfGreen = double(green) / 255;
    m_dfAlpha = double(alpha) / 255;
}

double wxGISColor::GetRed() const
{
    return m_dfRed; 
}

double wxGISColor::GetBlue() const
{
    return m_dfBlue;
}

double wxGISColor::GetGreen() const
{
    return m_dfGreen;
}

double wxGISColor::GetAlpha() const
{
    return m_dfAlpha;
}

wxGISColor wxGISColor::MakeRandom(ChannelType low)
{
    int random_number1 = (rand() % 255 - low); 
	int random_number2 = (rand() % 255 - low); 
	int random_number3 = (rand() % 255 - low); 

    return wxGISColor(low + random_number1, low + random_number2, low + random_number3, 255); 
} 

wxColour& wxGISColor::operator=(const wxColour& Color)
{
    if (this == &Color)
        return *this;
    Set(Color.Red(), Color.Green(), Color.Blue(), Color.Alpha());
    return *this;
}

wxGISColor& wxGISColor::operator=(const wxGISColor& Color)
{
    if (this == &Color)
        return *this;
    Set(Color.Red(), Color.Green(), Color.Blue(), Color.Alpha());
    return *this;
}

wxString wxGISColor::GetAsString(long nFlags) const
{
    if (nFlags & wxC2S_HTML_SYNTAX)
    {
        return wxColour::GetAsString(nFlags);
    }
    else
    {
        return wxString::Format(wxT("%d,%d,%d,%d"), Red(), Green(), Blue(), Alpha());
    }
}

void wxGISColor::SetFromString(const wxString &sColorTxt)
{
    wxArrayString saColors = wxStringTokenize(sColorTxt, wxT(","), wxTOKEN_RET_EMPTY);
    if(saColors.GetCount() < 4)
        return;
    int nR = wxAtoi(saColors[0]);
    int nG = wxAtoi(saColors[1]);
    int nB = wxAtoi(saColors[2]);
    int nA = wxAtoi(saColors[3]);
    Set(nR, nG, nB, nA);
}

void wxGISColor::SetAlpha(ChannelType alpha)
{
    Set(Red(), Green(), Blue(), alpha);
}

void wxGISColor::SetRed(ChannelType red)
{
    Set(red, Green(), Blue(), Alpha());
}

void wxGISColor::SetGreen(ChannelType green)
{
    Set(Red(), green, Blue(), Alpha());
}

void wxGISColor::SetBlue(ChannelType blue)
{
    Set(Red(), Green(), blue, Alpha());
}
