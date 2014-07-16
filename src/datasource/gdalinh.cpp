/******************************************************************************
 * Project:  wxGIS
 * Purpose:  inherited from gdal and ogr classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2014 Dmitry Baryshnikov
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
#include "wxgis/datasource/gdalinh.h"
#include <wx/encconv.h>
//-----------------------------------------------------------------------------
// wxGISSpatialReference
//-----------------------------------------------------------------------------

wxGISSpatialReference wxNullSpatialReference;

IMPLEMENT_CLASS(wxGISSpatialReference, wxObject)

wxGISSpatialReference::wxGISSpatialReference(OGRSpatialReference *poSRS)
{
    m_refData = new wxGISSpatialReferenceRefData(poSRS);
    ((wxGISSpatialReferenceRefData *)m_refData)->Validate();

}

wxObjectRefData *wxGISSpatialReference::CreateRefData() const
{
    return new wxGISSpatialReferenceRefData();
}

wxObjectRefData *wxGISSpatialReference::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISSpatialReferenceRefData(*(wxGISSpatialReferenceRefData *)data);
}

bool wxGISSpatialReference::IsOk() const
{
    return m_refData != NULL && ((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS != NULL && ((wxGISSpatialReferenceRefData *)m_refData)->IsValid();//;
}

bool wxGISSpatialReference::operator == ( const wxGISSpatialReference& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISSpatialReferenceRefData*)m_refData == *(wxGISSpatialReferenceRefData*)obj.m_refData );
}


wxGISSpatialReference::operator OGRSpatialReference*() const
{
    return ((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS;
}

OGRSpatialReference* wxGISSpatialReference::Clone(void) const
{
    wxCHECK_MSG(((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS, NULL, wxT("The OGRSpatialReference pointer is null"));
    return ((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS->Clone();
}

OGRSpatialReference* wxGISSpatialReference::operator->(void) const
{
    return ((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS;
}

bool wxGISSpatialReference::IsSame(const wxGISSpatialReference& SpatialReference) const
{
    if(!SpatialReference.IsOk() || !IsOk())
        return false;
    return ((wxGISSpatialReferenceRefData *)m_refData)->m_poSRS->IsSame(SpatialReference) == 0 ? false : true;
}

//-----------------------------------------------------------------------------
// wxGISSpatialReferenceRefData
//-----------------------------------------------------------------------------
wxGISSpatialReferenceRefData::wxGISSpatialReferenceRefData(OGRSpatialReference *poSRS)
{
    wsSET(m_poSRS, poSRS);
}

wxGISSpatialReferenceRefData::~wxGISSpatialReferenceRefData(void)
{
    wsDELETE(m_poSRS);
}

wxGISSpatialReferenceRefData::wxGISSpatialReferenceRefData(const wxGISSpatialReferenceRefData& data) : wxObjectRefData()
{
    m_poSRS = data.m_poSRS;
}

bool wxGISSpatialReferenceRefData::operator == (const wxGISSpatialReferenceRefData& data) const
{
    wxCHECK_MSG(m_poSRS && data.m_poSRS, false, wxT("m_poSRS or data.m_poSRS is null"));
    return m_poSRS->IsSame(data.m_poSRS) == 0 ? false : true;
}

void wxGISSpatialReferenceRefData::Validate()
{
    if (!m_poSRS)
    {
        m_bIsValid = false;
        return;
    }

    OGRErr eErr = m_poSRS->Validate();
    if (eErr != OGRERR_CORRUPT_DATA)
        m_bIsValid = true;
    else
    {
        m_poSRS->morphFromESRI();
        eErr = m_poSRS->Validate();
        if (eErr != OGRERR_UNSUPPORTED_SRS)
            m_bIsValid = true;
        else
            m_bIsValid = false;
    }
}

bool wxGISSpatialReferenceRefData::IsValid(void) const
{
    return m_bIsValid;
}

//-----------------------------------------------------------------------------
// wxGISFeature
//-----------------------------------------------------------------------------
#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_USER_EXPORTED_OBJARRAY(wxGISFeatureArray);

IMPLEMENT_CLASS(wxGISFeature, wxObject)

wxGISFeature::wxGISFeature(OGRFeature *poFeature, const wxFontEncoding &oEncodingS, bool bRecodeToSystem)
{
    if (poFeature != NULL)
        m_refData = new wxGISFeatureRefData(poFeature, oEncodingS, bRecodeToSystem);
}

wxObjectRefData *wxGISFeature::CreateRefData() const
{
    return new wxGISFeatureRefData();
}

wxObjectRefData *wxGISFeature::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISFeatureRefData(*(wxGISFeatureRefData *)data);
}

bool wxGISFeature::IsOk() const
{
    return m_refData != NULL && ((wxGISFeatureRefData *)m_refData)->m_poFeature != NULL;
}

bool wxGISFeature::operator == ( const wxGISFeature& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISFeatureRefData*)m_refData == *(wxGISFeatureRefData*)obj.m_refData );
}

wxGISFeature::operator OGRFeature*() const
{
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature;
}

OGRErr wxGISFeature::SetFID(long nFID)
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetFID(nFID);
}

long wxGISFeature::GetFID(void) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFID();
}

wxDateTime wxGISFeature::GetFieldAsDateTime(const wxString &sFieldName) const
{
    int nIndex = GetFieldIndex(sFieldName);
    if(nIndex == -1)
        return wxInvalidDateTime;
    return GetFieldAsDateTime(nIndex);
}

wxDateTime wxGISFeature::GetFieldAsDateTime(int nIndex) const
{
	int year, mon, day, hour, min, sec, flag;
    if (GetFieldAsDateTime(nIndex, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
    {
        wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
        return dt;
    }
    return wxInvalidDateTime;
}

int wxGISFeature::GetFieldAsDateTime(int nIndex, int *pnYear, int *pnMonth, int *pnDay, int *pnHour, int *pnMinute, int *pnSecond, int *pnTZFlag) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, FALSE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDateTime(nIndex, pnYear, pnMonth, pnDay, pnHour, pnMinute, pnSecond, pnTZFlag);
}

double wxGISFeature::GetFieldAsDouble(int nIndex) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDouble(nIndex);
}

double wxGISFeature::GetFieldAsDouble (const wxString &sFieldName) const
{
    int nIndex = GetFieldIndex(sFieldName);
    if(nIndex == -1)
        return 0;
    return GetFieldAsDouble(nIndex);
}

wxArrayInt wxGISFeature::GetFieldAsIntegerList(int nIndex) const
{
    wxArrayInt ret;
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, ret, wxT("The OGRFeature pointer is null"));
    int nCount = 0;
    const int* paInts = ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsIntegerList(nIndex, &nCount);
    for (int i = 0; i < nCount; ++i)
    {
        ret.Add(paInts[i]);
    }
    return ret;
}

wxArrayDouble wxGISFeature::GetFieldAsDoubleList(int nIndex) const
{
    wxArrayDouble ret;
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, ret, wxT("The OGRFeature pointer is null"));
    int nCount = 0;
    const double* paDoubles = ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDoubleList(nIndex, &nCount);
    for (int i = 0; i < nCount; ++i)
    {
        ret.Add(paDoubles[i]);
    }
    return ret;
}

wxArrayString wxGISFeature::GetFieldAsStringList(int nIndex) const
{
    wxArrayString ret;
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, ret, wxT("The OGRFeature pointer is null"));

    char** papszLinkList = ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsStringList(nIndex);
    for (int i = 0; papszLinkList[i] != NULL; ++i)
    {
        ret.Add(EncodeString(papszLinkList[i], ((wxGISFeatureRefData *)m_refData)->m_oEncoding));
    }

    return ret;
}


wxString wxGISFeature::GetFieldAsString(const wxString &sFieldName) const
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return wxEmptyString;
    return GetFieldAsString(nField);
}

const char* wxGISFeature::GetFieldAsChar(int nField) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, NULL, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsString(nField);
}

wxString wxGISFeature::GetFieldAsString(int nField) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxEmptyString, wxT("The OGRFeature pointer is null"));

	OGRFieldDefn* pDef = ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldDefnRef(nField);
    wxCHECK_MSG(pDef, wxEmptyString, wxT("The OGRFeature definition is null"));

    wxString sOut;
	switch(pDef->GetType())
	{
	case OFTDate:
		{
            wxDateTime dt = GetFieldAsDateTime(nField);
            if(dt.IsValid())
            {
				sOut = dt.FormatDate();//.Format(_("%d-%m-%Y"));
            }

            if (sOut == wxEmptyString)
            {
                sOut = wxT("<NULL>");
            }
		}
        break;
	case OFTTime:
		{
            wxDateTime dt = GetFieldAsDateTime(nField);
            if (dt.IsValid())
            {
				sOut = dt.FormatTime();//.Format(_("%H:%M:%S"));
            }

            if (sOut == wxEmptyString)
            {
                sOut = wxT("<NULL>");
            }
        }
        break;
	case OFTDateTime:
		{
            wxDateTime dt = GetFieldAsDateTime(nField);
            if (dt.IsValid())
            {
                sOut = dt.Format();//.Format(_("%d-%m-%Y %H:%M:%S"));
            }

            if (sOut == wxEmptyString)
            {
                sOut = wxT("<NULL>");
            }
        }
        break;
    case OFTReal:
		sOut = wxString::Format(wxT("%.12f"), GetFieldAsDouble(nField));
        break;
    case OFTInteger:
		sOut = wxString::Format(wxT("%d"), GetFieldAsInteger(nField));
        break;
	case OFTRealList:
		{
			wxArrayDouble DblLst = GetFieldAsDoubleList(nField);
            for (size_t i = 0; i < DblLst.GetCount(); ++i)
			{
                sOut += wxString::Format(wxT("%.12f;"), DblLst[i]);
			}
		}
		break;
	case OFTIntegerList:
		{
			wxArrayInt IntLst = GetFieldAsIntegerList(nField);
            for (size_t i = 0; i < IntLst.GetCount(); ++i)
			{
                sOut += wxString::Format(wxT("%.d;"), IntLst[i]);
			}
		}
		break;
	case OFTStringList:
		{
			wxArrayString StringLst = GetFieldAsStringList(nField);
            for (size_t i = 0; i < StringLst.GetCount(); ++i)
			{
                sOut.Append(StringLst[i]);
                sOut.Append(wxT(";"));
			}
		}
		break;
	default:
        {
            const char* pszStringData( ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsString(nField) );
            if (((wxGISFeatureRefData *)m_refData)->m_bRecodeToSystem)
            {
                //UTF->System
                CPLString szStr = CPLRecode(pszStringData, CPL_ENC_UTF8, wxLocale::GetSystemEncodingName().mb_str());
                sOut = EncodeString(szStr.c_str(), ((wxGISFeatureRefData *)m_refData)->m_oEncoding);
            }
            else
            {
                sOut = EncodeString(pszStringData, ((wxGISFeatureRefData *)m_refData)->m_oEncoding);
            }
        }
	}
	return sOut;
}

void wxGISFeature::SetField(int nIndex, int nValue)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, nValue);
}

void wxGISFeature::SetField(int nIndex, double dfValue)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, dfValue);
}

void wxGISFeature::SetField(int nIndex, const wxString &sValue)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
#ifdef CPL_RECODE_ICONV
    const char* szData = sValue.ToUTF8();// .mb_str(wxConvUTF8);
#else
    const char* szData = EncodeString(sValue, ((wxGISFeatureRefData *)m_refData)->m_oEncoding);
#endif //CPL_RECODE_ICONV
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, szData);
}

void wxGISFeature::SetField(int nIndex, const char* pszStr)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, pszStr);
}
//void wxGISFeature::SetField (int i, const wxArrayInt &anValues)
//{
//}
//
//void wxGISFeature::SetField (int i, int nCount, wxArrayDouble &adfValues)
//{
//}

void wxGISFeature::SetField(int nIndex, char **papszValues)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, papszValues);
}

void wxGISFeature::SetField(int nIndex, const wxArrayString &asValues)
{
    char **papszValues = NULL;
    for(size_t j = 0; j < asValues.GetCount(); ++j)
    {
        const char* szData = EncodeString(asValues[j], ((wxGISFeatureRefData *)m_refData)->m_oEncoding);
        papszValues = CSLAddString(papszValues, szData);
    }

    SetField(nIndex, papszValues);

    CSLDestroy( papszValues );
}

void wxGISFeature::SetField (const wxString &sFieldName, const wxArrayString &asValues)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, asValues);
}

void wxGISFeature::SetField(int nIndex, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nTZFlag)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, nYear, nMonth, nDay, nHour, nMinute, nSecond, nTZFlag);
}

void wxGISFeature::SetField(int nIndex, const wxDateTime &dt)
{
    SetField(nIndex, dt.GetYear(), dt.GetMonth() + 1, dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
}

void wxGISFeature::SetField (const wxString &sFieldName, int nValue)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, nValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, double dfValue)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, dfValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, const wxString &sValue)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, sValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nTZFlag)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, nYear, nMonth, nDay, nHour, nMinute, nSecond, nTZFlag);
}

void wxGISFeature::SetField(const wxString &sFieldName, const wxArrayInt &anValues)
{
    int nField = GetFieldIndex(sFieldName);
    if (nField == -1)
        return;
    SetField(nField, anValues);
}

void wxGISFeature::SetField(const wxString &sFieldName, const wxArrayDouble &adfValues)
{
    int nField = GetFieldIndex(sFieldName);
    if (nField == -1)
        return;
    SetField(nField, adfValues);
}

void wxGISFeature::SetField(int nIndex, const wxArrayInt &anValues)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));

    int *panValues = new int[anValues.GetCount()];
    for (size_t j = 0; j < anValues.GetCount(); ++j)
    {
        panValues[j] = anValues[j];
    }

    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, anValues.GetCount(), panValues);

    wxDELETE(panValues);
}

void wxGISFeature::SetField(int nIndex, const wxArrayDouble &adfValues)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));

    double *padfValues = new double[adfValues.GetCount()];
    for (size_t j = 0; j < adfValues.GetCount(); ++j)
    {
        padfValues[j] = adfValues[j];
    }

    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, adfValues.GetCount(), padfValues);

    wxDELETE(padfValues);
}

OGRErr wxGISFeature::SetGeometry(OGRGeometry* pGeom)
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetGeometryDirectly(pGeom);
}

OGRErr wxGISFeature::SetGeometry(const wxGISGeometry &Geom)
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetGeometryDirectly(Geom.Copy());
}

OGRErr wxGISFeature::SetGeometryDirectly(const wxGISGeometry &Geom)
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetGeometryDirectly(Geom);
}

wxGISGeometry wxGISFeature::GetGeometry(void) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxGISGeometry(), wxT("The OGRFeature pointer is null"));
    return wxGISGeometry(((wxGISFeatureRefData *)m_refData)->m_poFeature->GetGeometryRef(), false);
}

void wxGISFeature::SetStyleString(const wxString &sStyle)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetStyleString(sStyle.ToUTF8());
}

void wxGISFeature::StealGeometry(void)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->StealGeometry();
}

int wxGISFeature::GetFieldAsInteger(const wxString &sFieldName) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return 0;
    return GetFieldAsInteger(nField);
}

int wxGISFeature::GetFieldAsInteger(int nIndex) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsInteger(nIndex);
}

int wxGISFeature::GetFieldIndex(const wxString &sFieldName) const
{
    //TODO: check for russian field name encoding
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, -1, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldIndex(sFieldName.ToUTF8());
}

wxString wxGISFeature::GetFieldName(int nIndex) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxEmptyString, wxT("The OGRFeature pointer is null"));
    return wxString(((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldDefnRef(nIndex)->GetNameRef());
}

int wxGISFeature::GetFieldCount(void) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldCount();
}

OGRField* wxGISFeature::GetRawField(int nField) const
{
    wxCHECK_MSG(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetRawFieldRef(nField);
}

void wxGISFeature::SetField(int nIndex, OGRField* psField)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(nIndex, psField);
}

void wxGISFeature::SetEncoding(const wxFontEncoding &eEnc, bool bRecodeToSystem)
{
    wxCHECK_RET(m_refData && ((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->SetEncoding(eEnc, bRecodeToSystem);
}

//-----------------------------------------------------------------------------
// wxGISGeometry
//-----------------------------------------------------------------------------
wxGISGeometry wxNullGeometry;

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_USER_EXPORTED_OBJARRAY(wxGISGeometryArray);

IMPLEMENT_CLASS(wxGISGeometry, wxObject)

wxGISGeometry::wxGISGeometry(OGRGeometry *poGeom, bool bOwnGeom)
{
    m_refData = new wxGISGeometryRefData(poGeom, bOwnGeom);
}

wxObjectRefData *wxGISGeometry::CreateRefData() const
{
    return new wxGISGeometryRefData();
}

wxObjectRefData *wxGISGeometry::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISGeometryRefData(*(wxGISGeometryRefData *)data);
}

bool wxGISGeometry::IsOk() const
{
    return m_refData != NULL && ((wxGISGeometryRefData *)m_refData)->m_poGeom != NULL;
}

bool wxGISGeometry::operator == ( const wxGISGeometry& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISGeometryRefData*)m_refData == *(wxGISGeometryRefData*)obj.m_refData );
}

wxGISGeometry::operator OGRGeometry*() const
{
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom;
}

OGREnvelope wxGISGeometry::GetEnvelope(void) const
{
    OGREnvelope Env;
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, Env, wxT("OGRGeometry pointer is null"));
    ((wxGISGeometryRefData *)m_refData)->m_poGeom->getEnvelope(&Env);
    return Env;
}

OGRGeometry* wxGISGeometry::Copy(void) const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, NULL, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->clone();
}

OGRGeometry* wxGISGeometry::Steal(void)
{
    OGRGeometry* out = ((wxGISGeometryRefData *)m_refData)->m_poGeom;
    ((wxGISGeometryRefData *)m_refData)->m_poGeom = NULL;
    return out;
}

wxGISSpatialReference wxGISGeometry::GetSpatialReference() const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wxNullSpatialReference, wxT("OGRGeometry pointer is null"));
    OGRSpatialReference *pSpaRef = ((wxGISGeometryRefData *)m_refData)->m_poGeom->getSpatialReference();
    return wxGISSpatialReference(pSpaRef);
}

void wxGISGeometry::SetSpatialReference(const wxGISSpatialReference &SpaRef)
{
    wxCHECK_RET(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wxT("OGRGeometry pointer is null"));
    ((wxGISGeometryRefData *)m_refData)->m_poGeom->assignSpatialReference(SpaRef);
}

OGRPoint* wxGISGeometry::GetCentroid(void)
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, NULL, wxT("OGRGeometry pointer is null"));
    OGRPoint* pt = new OGRPoint();
    if(((wxGISGeometryRefData *)m_refData)->m_poGeom->Centroid(pt) == OGRERR_NONE)
    {
        wxGISSpatialReference SpaRef = GetSpatialReference();
        if(SpaRef.IsOk())
            pt->assignSpatialReference(SpaRef);
        return pt;
    }
    return NULL;
}


wxGISGeometry wxGISGeometry::Intersection(const wxGISGeometry &Geom) const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Intersection(Geom) );
}

wxGISGeometry wxGISGeometry::Union(const wxGISGeometry &Geom) const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Union(Geom) );
}

wxGISGeometry wxGISGeometry::UnionCascaded() const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->UnionCascaded() );
}

bool wxGISGeometry::Intersects(const wxGISGeometry &Geom) const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->Intersects(Geom) == 0 ? false : true;
}

wxGISGeometry wxGISGeometry::Buffer(double dfBuff, int nQuadSegs) const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Buffer(dfBuff, nQuadSegs) );
}

OGRwkbGeometryType wxGISGeometry::GetType() const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wkbUnknown, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->getGeometryType();
}

wxGISGeometry wxGISGeometry::Clone() const
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    wxGISGeometry geom(((wxGISGeometryRefData *)m_refData)->m_poGeom->clone());
    return geom;
}

bool wxGISGeometry::Project(const wxGISSpatialReference &SpaRef)
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->transformTo(SpaRef) == OGRERR_NONE;
}

bool wxGISGeometry::Project(OGRCoordinateTransformation* const poCT)
{
    wxCHECK_MSG(m_refData && ((wxGISGeometryRefData *)m_refData)->m_poGeom, false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->transform(poCT) == OGRERR_NONE;
}
