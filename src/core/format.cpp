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

#include "wxgis/core/format.h"
#include "wxgis/core/app.h"

#include <wx/numformatter.h>
#include <wx/utils.h>

wxString DoubleToString(double dVal, bool bIsLon)
{
	wxString znak;
	if(dVal < 0)
	{
		if(bIsLon) znak = wxString(_("W"));
		else znak = wxString(_("S"));
	}
	else
	{
		if(bIsLon) znak = wxString(_("E"));
		else znak = wxString(_("N"));
	}
	dVal = fabs(dVal);
	int grad = floor(dVal);
	int min = floor((dVal - grad) * 60);
	int sec = floor((dVal - grad - (double) min / 60) * 3600);
	wxString str;
	if(bIsLon)
		str.Printf(wxT("%.3d-%.2d-%.2d %s"), grad, min, sec, znak.c_str());
	else
		str.Printf(wxT("%.2d-%.2d-%.2d %s"), grad, min, sec, znak.c_str());
	return str;
}

double StringToDouble(const wxString &sVal, const wxString &sAsterisk)
{
	wxString buff;
	unsigned char counter = 0;
	int grad, min, sec;
	for(size_t i = 0; i < sVal.Len(); ++i)
	{
		wxUniChar ch = sVal[i];
		if(ch == '-' || ch == ' ')
		{
			switch(counter)
			{
				case 0:
				grad = wxAtoi(buff);
				break;
				case 1:
				min = wxAtoi(buff);
				break;
				case 2:
				sec = wxAtoi(buff);
				break;
			}
		}
		buff += ch;
	}
	int mul = -1;
	buff.Trim(true).Trim(false);
	if(buff.CmpNoCase(wxString(_("E"))) == 0 || buff.CmpNoCase(wxString(_("N"))) == 0)
		mul = 1;
	return ((double) grad + (double)min / 60 + (double)sec / 3600) * mul;
}

void FloatStringToCLoc(wxString & str)
{
    IApplication* const pApp = GetApplication();
    if(pApp && !pApp->GetDecimalPoint().IsSameAs('.'))
        str.Replace(pApp->GetDecimalPoint(), wxT("."));
}

void FloatStringFromCLoc(wxString & str)
{
    IApplication* const pApp = GetApplication();
    if(pApp && !pApp->GetDecimalPoint().IsSameAs('.'))
        str.Replace(wxT("."), pApp->GetDecimalPoint());
}

void SetFloatValue(wxXmlNode* pNode, const wxString &sAttrName, double dfVal)
{
    wxString sVal = wxString::Format(wxT("%f"), dfVal);
    FloatStringToCLoc(sVal);
    pNode->AddAttribute(sAttrName, sVal);
}

void SetFloatValue(wxXmlNode* pNode, const wxString &sAttrName, float dfVal)
{
    wxString sVal = wxString::Format(wxT("%f"), dfVal);
    FloatStringToCLoc(sVal);
    pNode->AddAttribute(sAttrName, sVal);
}

double GetFloatValue(const wxXmlNode* pNode, const wxString &sAttrName, double dfDefVal)
{
    if(!pNode)
        return dfDefVal;
    wxString sDefVal = wxString::Format(wxT("%f"), dfDefVal);
    FloatStringToCLoc(sDefVal);
    wxString sVal = pNode->GetAttribute(sAttrName, sDefVal);
    FloatStringFromCLoc(sVal);
    return wxAtof(sVal);
}

float GetFloatValue(const wxXmlNode* pNode, const wxString &sAttrName, float dfDefVal)
{
    if(!pNode)
        return dfDefVal;
    wxString sDefVal = wxString::Format(wxT("%f"), dfDefVal);
    FloatStringToCLoc(sDefVal);
    wxString sVal = pNode->GetAttribute(sAttrName, sDefVal);
    FloatStringFromCLoc(sVal);
    return wxAtof(sVal);
}

void SetDecimalValue(wxXmlNode* pNode, const wxString &sAttrName, long nVal)
{
    wxString sVal = wxString::Format(wxT("%d"), nVal);
    pNode->AddAttribute(sAttrName, sVal);
}

void SetDecimalValue(wxXmlNode* pNode, const wxString &sAttrName, int nVal)
{
    wxString sVal = wxString::Format(wxT("%d"), nVal);
    pNode->AddAttribute(sAttrName, sVal);
}

long GetDecimalValue(const wxXmlNode* pNode, const wxString &sAttrName, long nDefVal)
{
    if(!pNode)
        return nDefVal;
    wxString sDefVal = wxString::Format(wxT("%d"), nDefVal);
    wxString sVal = pNode->GetAttribute(sAttrName, sDefVal);
    return wxAtol(sVal);
}

int GetDecimalValue(const wxXmlNode* pNode, const wxString &sAttrName, int nDefVal)
{
    if(!pNode)
        return nDefVal;
    wxString sDefVal = wxString::Format(wxT("%d"), nDefVal);
    wxString sVal = pNode->GetAttribute(sAttrName, sDefVal);
    return wxAtoi(sVal);
}

void SetDateValue(wxXmlNode* pNode, const wxString &sAttrName, const wxDateTime &dtVal)
{
    pNode->AddAttribute(sAttrName, dtVal.Format(wxT("%d-%m-%Y %H:%M:%S")));
}

wxDateTime GetDateValue(const wxXmlNode* pNode, const wxString &sAttrName, const wxDateTime &dtDefVal)
{
    if(!pNode)
        return dtDefVal;
    const wxString sVal = pNode->GetAttribute(sAttrName, dtDefVal.Format(wxT("%d-%m-%Y %H:%M:%S")));
    wxDateTime dt;
    wxString::const_iterator itEnd;
    dt.ParseFormat(sVal, wxT("%d-%m-%Y %H:%M:%S"), dtDefVal, &itEnd);
    return dt;
}

wxJSONValue SetDateValue(const wxDateTime &dtVal)
{
    return wxJSONValue(dtVal.Format(wxT("%d-%m-%Y %H:%M:%S")));
}

wxDateTime GetDateValue(const wxJSONValue &val, const wxString &sAttrName, const wxDateTime &dtDefVal)
{
    if(val.IsNull())
        return dtDefVal;
    const wxString sVal = val.Get(sAttrName, wxJSONValue(dtDefVal.Format(wxT("%d-%m-%Y %H:%M:%S")))).AsString();
    wxDateTime dt;
    wxString::const_iterator itEnd;
    dt.ParseFormat(sVal, wxT("%d-%m-%Y %H:%M:%S"), dtDefVal, &itEnd);
    return dt;
}

void SetBoolValue(wxXmlNode* pNode, const wxString &sAttrName, bool bVal)
{
     pNode->AddAttribute(sAttrName, bVal == true ? wxT("t") : wxT("f"));
}

bool GetBoolValue(const wxXmlNode* pNode, const wxString &sAttrName, bool bDefVal)
{
    if(!pNode)
        return bDefVal;
    wxString sVal = pNode->GetAttribute(sAttrName, bDefVal == true ? wxT("t") : wxT("f"));
    if(sVal.IsSameAs(wxT("t"), false) || sVal.IsSameAs(wxT("1"), false) || sVal.IsSameAs(wxT("y"), false) || sVal.IsSameAs(wxT("yes"), false) || sVal.IsSameAs(wxT("true"), false))
        return true;
    return false;
}


wxString NumberScale(double dScaleRatio)
{
    return wxNumberFormatter::ToString(dScaleRatio, 2, wxNumberFormatter::Style_WithThousandsSep);
	//wxString str = wxString::Format(wxT("%.2f"), dScaleRatio);
	//int pos = str.Find(wxT("."));
	//if(pos == wxNOT_FOUND)
	//	pos = str.Len();
	//wxString res = str.Right(str.Len() - pos);
	//for(size_t i = 1; i < pos + 1; ++i)
	//{
	//	res.Prepend(str[pos - i]);
	//	if((i % 3) == 0)
	//		res.Prepend(wxT(" "));
	//}
	//return res;
}

wxString GetSubString(const wxString &sInputString, const wxString &sBegin, const wxString &sEnd, bool bAppendBegin)
{
    int pos = sInputString.Find(sBegin);
    if(pos == wxNOT_FOUND)
        return wxEmptyString;
    if(!bAppendBegin)
        pos += sBegin.Len();

    wxString sOut = sInputString.Right(sInputString.Len() - pos);
    pos = sOut.Find(sEnd);
    if(pos == wxNOT_FOUND)
        pos = sOut.Len();

    sOut = sOut.Left(pos);

    return sOut;
}

#define UNICODE_MAX 0x10FFFFul

wxString DecodeHTMLEntities(const wxString &sSrc)
{
    wxString in = sSrc;
    wxString out;
    int nPos = in.Find('&');
    while(nPos != wxNOT_FOUND)
    {
        out += in.Left(nPos);
        in = in.Right(in.Len() - nPos - 3);
        nPos = in.Find(';');
        unsigned long cp = wxHexToDec(in.Left(nPos));
        in = in.Right(in.Len() - nPos - 1);
        if(cp <= UNICODE_MAX)
        {
            out += wxUniChar(cp);
        }
        nPos = in.Find('&');
    }

    out += in;
	return out;
}

//-----------------------------------------------------------------------------
// wxGISCoordinatesFormat
//-----------------------------------------------------------------------------

//TODO: formt using geographic and projected spatial reference m -> deg and deg->m acording to format

wxGISCoordinatesFormat::wxGISCoordinatesFormat(void)
{
}

wxGISCoordinatesFormat::wxGISCoordinatesFormat(const wxString &sMask, bool bSwap)
{
	Create(sMask, bSwap);
}

void wxGISCoordinatesFormat::Create(const wxString &sMask, bool bSwap)
{
	m_bIsOk = false;
	m_sMask = sMask.Clone();
    //search and remove sign indicator [W]
	m_bSign = m_sMask.Find(wxT("[W]")) == wxNOT_FOUND ? false : true;
    m_sMask.Replace(_T("[W]"), _T(""));

    m_sMask.Replace(_T("tab"), _T("\t"));
    m_sMask.Replace(_T("TAB"), _T("\t"));
    m_sMask.Replace(_T("ret"), _T("\n"));
    m_sMask.Replace(_T("RET"), _T("\n"));

    // find [
    int pos = m_sMask.Find(_T("["));
    if(pos == wxNOT_FOUND)
        return;

	wxString sPart1 = m_sMask.Left(pos);
	m_sMask = m_sMask.Right(m_sMask.Len() - pos - 1);
    // find ]
    pos = m_sMask.Find(_T("]"));
    if(pos == wxNOT_FOUND)
        return;

    m_sDivider = m_sMask.Left(pos);
	wxString sPart2 = m_sMask.Right(m_sMask.Len() - pos - 1);

    //create format 1
    m_sFormatX = ParseString(sPart1, &m_CoordFormatX);
    //create format 2
    m_sFormatY = ParseString(sPart2, &m_CoordFormatY);

	m_bSwaped = bSwap;
	m_sMask = sMask.Clone();

	m_bIsOk = true;
}

wxGISCoordinatesFormat::~wxGISCoordinatesFormat()
{
}

wxString wxGISCoordinatesFormat::ParseString(const wxString &sMask, COORD_FORMAT* pCoordFormat)
{
    wxString sOut;//, sFormat(wxT("%")), sSubs;

    int nD(0), nM(0), nS(0);
    int nd(0), nm(0), ns(0);
    bool bPtSet(false);

    struct parts_present
    {
        bool bG, bM, bS;
    };
    parts_present pres = {false, false, false};

    for(size_t i = 0; i < sMask.Len(); ++i)
    {
        wxUniChar c = sMask[i];

        if(c == '.')
        {
            bPtSet = true;
            continue;
        }
        if(c == 'd' || c == 'D')
        {
            if(bPtSet)
                nd++;
            else
                nD++;
            continue;
        }
        if(c == 'm' || c == 'M')
        {
            if(nd)
                continue;
            if(bPtSet)
                nm++;
            else
                nM++;
            if(nD)
            {
                sOut += FormatToken(nD, nd);
                pres.bG = true;
                nD = nd = 0;
            }
            continue;
        }
        if(c == 's' || c == 'S')
        {
            if(nd || nm)
                continue;
            if(bPtSet)
                ns++;
            else
                nS++;

            if(nD)
            {
                sOut += FormatToken(nD, nd);
                pres.bG = true;
                nD = nd = 0;
            }
            else if(nM)
            {
                sOut += FormatToken(nM, nm);
                pres.bM = true;
                nM = nm = 0;
            }
            continue;
        }
//
        if(nD > 0)
        {
            sOut += FormatToken(nD, nd);
            pres.bG = true;
            nD = nd = 0;
        }
        if(nM > 0)
        {
            sOut += FormatToken(nM, nm);
            pres.bM = true;
            nM = nm = 0;
        }
        if(nS > 0)
        {
            sOut += FormatToken(nS, ns);
            pres.bS = true;
            nS = ns = 0;
        }
        //add other string
        sOut += c;
    }
    //form str
    if(nD > 0)
    {
        sOut += FormatToken(nD, nd);
        pres.bG = true;
    }
    if(nM > 0)
    {
        sOut += FormatToken(nM, nm);
        pres.bM = true;
    }
    if(nS > 0)
    {
        sOut += FormatToken(nS, ns);
        pres.bS = true;
    }

    if(pres.bG && pres.bM & pres.bS)
        *pCoordFormat = DMS;
    else if(!pres.bG && pres.bM & pres.bS)
        *pCoordFormat = MS;
    else if(!pres.bG && !pres.bM & pres.bS)
        *pCoordFormat = S;
    else if(pres.bG && !pres.bM & !pres.bS)
        *pCoordFormat = D;
    else if(pres.bG && pres.bM & !pres.bS)
        *pCoordFormat = DM;
    else if(!pres.bG && pres.bM & !pres.bS)
        *pCoordFormat = M;
    else
         *pCoordFormat = D;

    return sOut;
}

wxString wxGISCoordinatesFormat::FormatString(double dCoord, const wxString &sFormat, COORD_FORMAT nCoordFormat, bool bLat)
{
    wxString sOut, sSign;

    int nZnak(1);
    if(dCoord < 0)
    {
        nZnak = -1;
        dCoord = fabs(dCoord);
    }

    if(m_bSign)
    {
        if(bLat)
            if(nZnak > 0)
				sSign = wxString(_("N"));
            else
				sSign = wxString(_("S"));
        else
            if(nZnak > 0)
                sSign = wxString(_("E"));
            else
                sSign = wxString(_("W"));
    }

    switch(nCoordFormat)
    {
        case D:
            if(m_bSign)
            {
                sOut.Printf(sFormat, dCoord);
                sOut += sSign;
            }
            else
                sOut.Printf(sFormat, dCoord * nZnak);
            return sOut;
        case DM:
            //ectract parts
            {
                int nGrad = floor(dCoord);
                double dMin = (dCoord - nGrad) * 60;
                int nMin = dMin;
                bool bMinFloat = m_sMask.Lower().Find(wxT("m.m")) == wxNOT_FOUND ? false : true;

                if(m_bSign)
                {
                    if(bMinFloat)
                        sOut.Printf(sFormat, nGrad, dMin);
                    else
                        sOut.Printf(sFormat, nGrad, nMin);
					sOut += wxString(wxT(" ")) + sSign;
                }
                else
				{
                    if(bMinFloat)
                        sOut.Printf(sFormat, nGrad * nZnak, dMin);
                    else
                        sOut.Printf(sFormat, nGrad * nZnak, nMin);
				}
                return sOut;
            }
        case DMS:
            //ectract parts
            {
                int nGrad = floor(dCoord);
                double dMin = (dCoord - nGrad) * 60;
                int nMin = floor(dMin);
                double dSec = (dMin - nMin) * 60;
                int nSec = dSec;
                bool bSecFloat = m_sMask.Lower().Find(wxT("s.s")) == wxNOT_FOUND ? false : true;

                if(m_bSign)
                {
                    if(bSecFloat)
                        sOut.Printf(sFormat, nGrad, nMin, dSec);
                    else
                        sOut.Printf(sFormat, nGrad, nMin, nSec);
                    sOut += sSign;
                }
                else
                    if(bSecFloat)
                        sOut.Printf(sFormat, nGrad * nZnak, nMin, dSec);
                    else
                        sOut.Printf(sFormat, nGrad * nZnak, nMin, nSec);
                return sOut;
            }
        case M:
            if(m_bSign)
            {
                sOut.Printf(sFormat, dCoord * 60);
                sOut += sSign;
            }
            else
                sOut.Printf(sFormat, dCoord * 60 * nZnak);
            return sOut;
        case MS:
            //ectract parts
            {
                double dMin = dCoord * 60;
                int nMin = floor(dMin);
                double dSec = (dMin - nMin) * 60;
                int nSec = dSec;
                bool bSecFloat = m_sMask.Lower().Find(wxT("s.s")) == wxNOT_FOUND ? false : true;

                if(m_bSign)
                {
                    if(bSecFloat)
                        sOut.Printf(sFormat, nMin, dSec);
                    else
                        sOut.Printf(sFormat, nMin, nSec);
                    sOut += sSign;
                }
                else
                    if(bSecFloat)
                        sOut.Printf(sFormat, nMin * nZnak, dSec);
                    else
                        sOut.Printf(sFormat, nMin * nZnak, nSec);
                return sOut;
            }
        case S:
            if(m_bSign)
            {
                sOut.Printf(sFormat, dCoord * 3600);
                sOut += sSign;
            }
            else
                sOut.Printf(sFormat, dCoord * 3600 * nZnak);
            return sOut;
		default:
            return sOut;
    }
}

wxString wxGISCoordinatesFormat::FormatToken(int nValHigh, int nValLow)
{
    wxString sSubs, sFormat(wxT("%"));
    if(nValLow > 0)
    {
        sSubs.Printf(wxT("0%d.%d"), nValHigh, nValLow);
        sFormat += sSubs;
        sFormat += wxString(wxT("f"));
    }
    else if(nValHigh > 0)
    {
        sSubs.Printf(wxT("0%d"), nValHigh);
        sFormat += sSubs;
        sFormat += wxString(wxT("d"));
    }
    return sFormat;
}

wxString wxGISCoordinatesFormat::Format(double dX, double dY)
{
    //strX
    wxString strX = FormatString(dX, m_sFormatX, m_CoordFormatX, false);
    //strY
    wxString strY = FormatString(dY, m_sFormatY, m_CoordFormatY, true);
    wxString str;
    if(m_bSwaped)
    {
        str = strY + m_sDivider;
        str += strX;
    }
    else
    {
        str = strX + m_sDivider;
        str += strY;
    }
    return str;
}

