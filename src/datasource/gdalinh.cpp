/******************************************************************************
 * Project:  wxGIS
 * Purpose:  inherited from gdal and ogr classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012,2013 Bishop
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
#include "wxgis/datasource/gdalinh.h"

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
// wxGISFeature
//-----------------------------------------------------------------------------
#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_USER_EXPORTED_OBJARRAY(wxGISFeatureArray);

IMPLEMENT_CLASS(wxGISFeature, wxObject)

wxGISFeature::wxGISFeature(OGRFeature *poFeature, const wxFontEncoding &oEncodingS)
{
    m_refData = new wxGISFeatureRefData(poFeature, oEncodingS);
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
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetFID(nFID);
}

long wxGISFeature::GetFID(void) const
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFID();
}

wxDateTime wxGISFeature::GetFieldAsDateTime(const wxString &sFieldName)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxInvalidDateTime, wxT("The OGRFeature pointer is null"));
    int nIndex = GetFieldIndex(sFieldName);
    if(nIndex == -1)
        return wxInvalidDateTime;
	int year, mon, day, hour, min, sec, flag;
	if(GetFieldAsDateTime(nIndex, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
    {
        wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
        return dt;
    }
    return wxInvalidDateTime;
}

int wxGISFeature::GetFieldAsDateTime (int i, int *pnYear, int *pnMonth, int *pnDay, int *pnHour, int *pnMinute, int *pnSecond, int *pnTZFlag)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, FALSE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDateTime(i, pnYear, pnMonth, pnDay, pnHour, pnMinute, pnSecond, pnTZFlag);
}

double wxGISFeature::GetFieldAsDouble (int i)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDouble(i);
}

double wxGISFeature::GetFieldAsDouble (const wxString &sFieldName)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    int nIndex = GetFieldIndex(sFieldName);
    if(nIndex == -1)
        return 0;
    return GetFieldAsDouble(nIndex);
}

const int * wxGISFeature::GetFieldAsIntegerList (int i, int *pnCount)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, NULL, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsIntegerList(i, pnCount);
}

const double * wxGISFeature::GetFieldAsDoubleList (int i, int *pnCount)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, NULL, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsDoubleList(i, pnCount);
}

char ** wxGISFeature::GetFieldAsStringList (int i) const
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, NULL, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsStringList(i);
}

wxString wxGISFeature::GetFieldAsString(const wxString &sFieldName)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxEmptyString, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return wxEmptyString;
    return GetFieldAsString(nField);
}

wxString wxGISFeature::GetFieldAsString(int nField)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxEmptyString, wxT("The OGRFeature pointer is null"));

	OGRFieldDefn* pDef = ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldDefnRef(nField);
    wxCHECK_MSG(pDef, wxEmptyString, wxT("The OGRFeature definition is null"));

    wxString sOut;
	switch(pDef->GetType())
	{
	case OFTDate:
		{
			int year, mon, day, hour, min, sec, flag;
			if(GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
				    sOut = dt.FormatDate();//.Format(_("%d-%m-%Y"));
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTTime:
		{
			int year, mon, day, hour, min, sec, flag;
			if(GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
				    sOut = dt.FormatTime();//.Format(_("%H:%M:%S"));
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTDateTime:
		{
			int year, mon, day, hour, min, sec, flag;
			if(GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
                    sOut = dt.Format();//.Format(_("%d-%m-%Y %H:%M:%S"));
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTReal:
		sOut = wxString::Format(wxT("%.12f"), GetFieldAsDouble(nField));
        if(sOut == wxEmptyString)
            sOut = wxT("<NULL>");
        break;
	case OFTRealList:
		{
			int nCount(0);
			const double* pDblLst = GetFieldAsDoubleList(nField, &nCount);
			for(int i = 0; i < nCount; ++i)
			{
				sOut += wxString::Format(wxT("%.12f;"), pDblLst[i]);
			}
		}
		break;
	case OFTIntegerList:
		{
			int nCount(0);
			const int* pIntLst = GetFieldAsIntegerList(nField, &nCount);
			for(int i = 0; i < nCount; ++i)
			{
				sOut += wxString::Format(wxT("%.d;"), pIntLst[i]);
			}
		}
		break;
	case OFTStringList:
		{
			char** papszLinkList = GetFieldAsStringList(nField);
			for(int i = 0; papszLinkList[i] != NULL; ++i )
			{
                if(((wxGISFeatureRefData *)m_refData)->m_oEncoding <= wxFONTENCODING_DEFAULT)
                {
                    sOut = wxString(papszLinkList[i], wxConvLocal);
                }
                else if(((wxGISFeatureRefData *)m_refData)->m_oEncoding == wxFONTENCODING_UTF8)
                    sOut = wxString(papszLinkList[i], wxConvUTF8);
                else
                {
                    sOut = wxString(papszLinkList[i], wxCSConv(((wxGISFeatureRefData *)m_refData)->m_oEncoding));
                    if(sOut.IsEmpty())
                        sOut = wxString(papszLinkList[i], wxConvLocal);
                }
				sOut += wxString(wxT(";"));
			}
		}
		break;
	default:
        {
            CPLString pszStringData( ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsString(nField) );
            if(!pszStringData.empty())
            {
                if(((wxGISFeatureRefData *)m_refData)->m_oEncoding <= wxFONTENCODING_DEFAULT)
                {
                    sOut = wxString(pszStringData, wxConvLocal);
                }
                else if(((wxGISFeatureRefData *)m_refData)->m_oEncoding == wxFONTENCODING_UTF8)
                    sOut = wxString(pszStringData, wxConvUTF8);
                else
                {
                    sOut = wxString(pszStringData, wxCSConv(((wxGISFeatureRefData *)m_refData)->m_oEncoding));
                    if(sOut.IsEmpty())
                        sOut = wxString(pszStringData, wxConvLocal);
                }
            }
        }
	}
	return sOut;
}

void wxGISFeature::SetField (int i, int nValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(i, nValue);
}

void wxGISFeature::SetField (int i, double dfValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(i, dfValue);
}

void wxGISFeature::SetField (int i, const wxString &sValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    CPLString szData(sValue.mb_str(wxCSConv(((wxGISFeatureRefData *)m_refData)->m_oEncoding)));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(i, szData);
}

//void wxGISFeature::SetField (int i, const wxArrayInt &anValues)
//{
//}
//
//void wxGISFeature::SetField (int i, int nCount, wxArrayDouble &adfValues)
//{
//}

void wxGISFeature::SetField (int i, const wxArrayString &asValues)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));

    char **papszValues = NULL;
    for(size_t j = 0; j < asValues.GetCount(); ++j)
    {
        CPLString szData(asValues[j].mb_str(wxCSConv(((wxGISFeatureRefData *)m_refData)->m_oEncoding)));
        papszValues = CSLAddString(papszValues, szData);
    }

    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(i, papszValues);
    CSLDestroy( papszValues );
}

void wxGISFeature::SetField (const wxString &sFieldName, const wxArrayString &asValues)
{
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, asValues);
}

void wxGISFeature::SetField (int i, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nTZFlag)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetField(i, nYear, nMonth, nDay, nHour, nMinute, nSecond, nTZFlag);
}

void wxGISFeature::SetField (const wxString &sFieldName, int nValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, nValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, double dfValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, dfValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, const wxString &sValue)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, sValue);
}

void wxGISFeature::SetField (const wxString &sFieldName, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nTZFlag)
{
    wxCHECK_RET(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return;
    SetField(nField, nYear, nMonth, nDay, nHour, nMinute, nSecond, nTZFlag);
}

OGRErr wxGISFeature::SetGeometry(OGRGeometry* pGeom)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetGeometryDirectly(pGeom);
}

OGRErr wxGISFeature::SetGeometry(const wxGISGeometry &Geom)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, OGRERR_INVALID_HANDLE, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->SetGeometryDirectly(Geom.Copy());
}

wxGISGeometry wxGISFeature::GetGeometry(void) const
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxGISGeometry(), wxT("The OGRFeature pointer is null"));
    return wxGISGeometry(((wxGISFeatureRefData *)m_refData)->m_poFeature->GetGeometryRef(), false);
}

int wxGISFeature::GetFieldAsInteger(const wxString &sFieldName)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    int nField = GetFieldIndex(sFieldName);
    if(nField == -1)
        return 0;
    return GetFieldAsInteger(nField);
}

int wxGISFeature::GetFieldAsInteger(int i)
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldAsInteger(i);
}

int wxGISFeature::GetFieldIndex(const wxString &sFieldName)
{
    //TODO: check for russian field name encoding
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, -1, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldIndex(sFieldName.mb_str());
}

wxString wxGISFeature::GetFieldName(int nIndex) const
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, wxEmptyString, wxT("The OGRFeature pointer is null"));
    return wxString(((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldDefnRef(nIndex)->GetNameRef());
}

int wxGISFeature::GetFieldCount(void) const
{
    wxCHECK_MSG(((wxGISFeatureRefData *)m_refData)->m_poFeature, 0, wxT("The OGRFeature pointer is null"));
    return ((wxGISFeatureRefData *)m_refData)->m_poFeature->GetFieldCount();
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
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, Env, wxT("OGRGeometry pointer is null"));
    ((wxGISGeometryRefData *)m_refData)->m_poGeom->getEnvelope(&Env);
    return Env;
}

OGRGeometry* wxGISGeometry::Copy(void) const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, NULL, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->clone();
}

wxGISSpatialReference wxGISGeometry::GetSpatialReference() const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, wxNullSpatialReference, wxT("OGRGeometry pointer is null"));
    OGRSpatialReference *pSpaRef = ((wxGISGeometryRefData *)m_refData)->m_poGeom->getSpatialReference();
    return wxGISSpatialReference(pSpaRef);
}

void wxGISGeometry::SetSpatialReference(const wxGISSpatialReference &SpaRef)
{
    wxCHECK_RET(((wxGISGeometryRefData *)m_refData)->m_poGeom, wxT("OGRGeometry pointer is null"));
    ((wxGISGeometryRefData *)m_refData)->m_poGeom->assignSpatialReference(SpaRef);
}

OGRPoint* wxGISGeometry::GetCentroid(void)
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, NULL, wxT("OGRGeometry pointer is null"));
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
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Intersection(Geom) );
}

wxGISGeometry wxGISGeometry::Union(const wxGISGeometry &Geom) const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Union(Geom) );
}

wxGISGeometry wxGISGeometry::UnionCascaded() const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->UnionCascaded() );
}

bool wxGISGeometry::Intersects(const wxGISGeometry &Geom) const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom && Geom.IsOk(), false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->Intersects(Geom) == 0 ? false : true;
}

wxGISGeometry wxGISGeometry::Buffer(double dfBuff, int nQuadSegs) const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, wxGISGeometry(), wxT("OGRGeometry pointer is null"));
    return wxGISGeometry( ((wxGISGeometryRefData *)m_refData)->m_poGeom->Buffer(dfBuff, nQuadSegs) );
}

OGRwkbGeometryType wxGISGeometry::GetType() const
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, wkbUnknown, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->getGeometryType();
}

bool wxGISGeometry::Project(const wxGISSpatialReference &SpaRef)
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->transformTo(SpaRef) == OGRERR_NONE;
}

bool wxGISGeometry::Project(OGRCoordinateTransformation* const poCT)
{
    wxCHECK_MSG(((wxGISGeometryRefData *)m_refData)->m_poGeom, false, wxT("OGRGeometry pointer is null"));
    return ((wxGISGeometryRefData *)m_refData)->m_poGeom->transform(poCT) == OGRERR_NONE;
}