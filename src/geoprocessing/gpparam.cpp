/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  parameter class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpdomain.h"

#include <wx/tokenzr.h>

#define MULTI_PARAM_SEPARATOR wxString(wxT("#"))

//---------------------------------------------------------------------------
// wxGISGPParameter
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISGPParamEvent, wxEvent)

wxDEFINE_EVENT( wxGPPARAM_CHANGED, wxGISGPParamEvent );
wxDEFINE_EVENT( wxGPPARAM_MSG_SET, wxGISGPParamEvent );
wxDEFINE_EVENT( wxGPPARAM_DOMAIN_ADDVAL, wxGISGPParamEvent );
wxDEFINE_EVENT( wxGPPARAM_DOMAIN_CLEAR, wxGISGPParamEvent );

//---------------------------------------------------------------------------
// wxGISGPParameter
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISGPParameter, wxObject)

wxGISGPParameter::wxGISGPParameter()
{
    m_sName = wxEmptyString;
    m_sDisplayName = wxEmptyString;
    m_DataType = enumGISGPParamDTUnknown;
    m_ParameterType = enumGISGPParameterTypeOptional;

    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
	m_nSelection = 0;
    m_nMsgType = wxGISEnumGPMessageUnknown;
    m_sMessage = wxEmptyString;
}

wxGISGPParameter::wxGISGPParameter(const wxString &sName, const wxString &sDisplayName, wxGISEnumGPParameterType eType, wxGISEnumGPParameterDataType eDataType)
{
    m_sName = sName;
    m_sDisplayName = sDisplayName;
    m_DataType = eDataType;
    m_ParameterType = eType;

    m_bAltered = false;
    m_bHasBeenValidated = false;
    m_bIsValid = false;
    m_pDomain = NULL;
	m_nSelection = 0;
    m_nMsgType = wxGISEnumGPMessageUnknown;
    m_sMessage = wxEmptyString;
}

wxGISGPParameter::~wxGISGPParameter()
{
    wxDELETE(m_pDomain);
}

void wxGISGPParameter::SetId(size_t nId)
{
    m_nId = nId;
}

size_t wxGISGPParameter::GetId(void) const
{
    return m_nId;
}

bool wxGISGPParameter::GetAltered(void) const
{
    return m_bAltered;
}

void wxGISGPParameter::SetAltered(bool bAltered)
{
    m_bAltered = bAltered;
}

bool wxGISGPParameter::GetHasBeenValidated(void) const
{
    return m_bHasBeenValidated;
}

void wxGISGPParameter::SetHasBeenValidated(bool bHasBeenValidated)
{
    m_bHasBeenValidated = bHasBeenValidated;
}

bool wxGISGPParameter::IsValid(void) const
{
    return m_bIsValid;
}

void wxGISGPParameter::SetValid(bool bIsValid)
{
    m_bIsValid = bIsValid;
}

wxString wxGISGPParameter::GetName(void) const
{
    return m_sName;
}

void wxGISGPParameter::SetName(const wxString &sName)
{
    m_sName = sName;
}

wxString wxGISGPParameter::GetDisplayName(void) const
{
    return m_sDisplayName;
}

void wxGISGPParameter::SetDisplayName(const wxString &sDisplayName)
{
    m_sDisplayName = sDisplayName;
}

wxGISEnumGPParameterDataType wxGISGPParameter::GetDataType(void) const
{
    return m_DataType;
}

void wxGISGPParameter::SetDataType(wxGISEnumGPParameterDataType nType)
{
    m_DataType = nType;
}

wxGISEnumGPParameterDirection wxGISGPParameter::GetDirection(void) const
{
    return m_Direction;
}

void wxGISGPParameter::SetDirection(wxGISEnumGPParameterDirection nDirection)
{
    m_Direction = nDirection;
}

wxGISEnumGPParameterType wxGISGPParameter::GetParameterType(void) const
{
    return m_ParameterType;
}

void wxGISGPParameter::SetParameterType(wxGISEnumGPParameterType nType)
{
    m_ParameterType = nType;
}

wxVariant wxGISGPParameter::GetValue(void) const
{
    return m_Value;
}

void wxGISGPParameter::SetValue(const wxVariant &Val)
{
    m_bHasBeenValidated = false;
    m_Value = Val;
    AddEvent(wxGISGPParamEvent(m_nId, wxGPPARAM_CHANGED, m_Value, m_Value.GetName()));
}

wxGISGPValueDomain* wxGISGPParameter::GetDomain(void) const
{
    return m_pDomain;
}

void wxGISGPParameter::SetDomain(wxGISGPValueDomain* pDomain)
{
    if(m_pDomain != NULL)
    {
        m_pDomain->Clear();
        wxDELETE(m_pDomain);
    }
    m_pDomain = pDomain;
    m_pDomain->SetParent(static_cast<IGISGPDomainParent*>(this));

    //set value from default value of domain
    if(m_Value.IsNull())
        SetSelDomainValue(0);
}

wxString wxGISGPParameter::GetMessage(void) const
{
    return m_sMessage;
}

wxGISEnumGPMessageType wxGISGPParameter::GetMessageType(void) const
{
    return m_nMsgType;
}

void wxGISGPParameter::SetMessage(wxGISEnumGPMessageType nType, const wxString &sMsg)
{
    m_sMessage = sMsg;
    m_nMsgType = nType;
    wxGISGPParamEvent event(m_nId, wxGPPARAM_MSG_SET, m_Value, m_Value.GetName());
    AddEvent(event);
}

int wxGISGPParameter::GetSelDomainValue(void) const
{
	return m_nSelection;
}

void wxGISGPParameter::SetSelDomainValue(int nNewSelection)
{
    m_bHasBeenValidated = false;
	m_nSelection = nNewSelection;
	if(m_pDomain)
	{
		switch(m_DataType)
		{
		case enumGISGPParamDTBool:
		case enumGISGPParamDTInteger:
		case enumGISGPParamDTDouble:
		case enumGISGPParamDTText:
		case enumGISGPParamDTSpatRef:
		case enumGISGPParamDTQuery:
        case enumGISGPParamDTFieldAnyChoice:
        case enumGISGPParamDTFieldStringChoice:
        case enumGISGPParamDTFieldIntegerChoice:
        case enumGISGPParamDTFieldRealChoice:
        case enumGISGPParamDTFieldDateChoice:
        case enumGISGPParamDTFieldTimeChoice:
        case enumGISGPParamDTFieldDateTimeChoice:
        case enumGISGPParamDTFieldBinaryChoice:
        case enumGISGPParamDTStringChoice:
        case enumGISGPParamDTIntegerChoice:
        case enumGISGPParamDTDoubleChoice:
        case enumGISGPParamDTStringChoiceEditable:
            if (m_pDomain->GetCount() > 0)
                SetValue(m_pDomain->GetValue(nNewSelection));
			break;
		case enumGISGPParamDTStringList:
		case enumGISGPParamDTPathArray:
		case enumGISGPParamDTPath:
            {
                //change m_Value ext to filters
                wxString sData = GetValue();
                wxGISGPGxObjectDomain* poDomain = wxDynamicCast(GetDomain(), wxGISGPGxObjectDomain);
                if(poDomain)
                {
                    wxFileName oName(sData);
                    if(oName.IsOk())
                    {
                        if(!oName.HasExt()) //set sel domain
                        {
                            wxGxObjectFilter* poFilter = poDomain->GetFilter(GetSelDomainValue());
                            oName.SetExt(poFilter->GetExt());
                            SetValue(wxVariant(oName.GetFullPath(), wxT("path")));
                        }
                    }
                }
            }
			break;        
		case enumGISGPParamDTUnknown:
		default:
			break;        
		}
	}
}

wxString wxGISGPParameter::GetAsString(void) const
{
    wxString sStrPar = m_Value.MakeString();
	sStrPar.Replace(wxT("\""), wxT("\\\""));
    return sStrPar;
}

bool wxGISGPParameter::SetFromString(wxString &sParam)
{
	sParam.Replace(wxT("\\\""), wxT("\""));

    switch(m_DataType)
    {
    case enumGISGPParamDTBool:
        m_Value = wxVariant((bool)(wxAtoi(sParam) == TRUE));
        break;        
	case enumGISGPParamDTInteger:
		m_Value = wxVariant(wxAtoi(sParam));
        break;        
	case enumGISGPParamDTDouble:
		m_Value = wxVariant(wxAtof(sParam));
        break;        
	case enumGISGPParamDTText:
	case enumGISGPParamDTSpatRef:
	case enumGISGPParamDTPath:
	case enumGISGPParamDTPathArray:
    case enumGISGPParamDTStringChoice:
        m_Value = wxVariant(sParam);
        break;        
	case enumGISGPParamDTIntegerChoice:
        m_Value = wxVariant(wxAtoi(sParam));
        break; 
	case enumGISGPParamDTDoubleChoice:  
        m_Value = wxVariant(wxAtof(sParam));
        break; 
    case enumGISGPParamDTStringList:
	case enumGISGPParamDTIntegerList:
	case enumGISGPParamDTDoubleList:
		m_Value = wxStringTokenize(sParam, wxT(";"), wxTOKEN_RET_EMPTY);
        break;        
    case enumGISGPParamDTUnknown:
    default:
        m_Value = wxVariant(sParam);
        break;        
    }

	if(m_pDomain)
	{
		int nPos = m_pDomain->GetPosByValue(m_Value);
		if(nPos != wxNOT_FOUND)
			SetSelDomainValue(nPos);
	}

    return true;
}

void wxGISGPParameter::AddDependency(const wxString &sParamName)
{
    m_saDependencies.Add(sParamName);
}

wxArrayString wxGISGPParameter::GetDependences(void) const
{
    return m_saDependencies;
}

void wxGISGPParameter::OnValueAdded(const wxVariant &Value, const wxString &sName)
{
    wxGISGPParamEvent event(m_nId, wxGPPARAM_DOMAIN_ADDVAL, Value, sName);
    AddEvent(event);
}

void wxGISGPParameter::OnCleared(void)
{
    wxGISGPParamEvent event(m_nId, wxGPPARAM_DOMAIN_CLEAR, wxNullVariant, wxEmptyString);
    AddEvent(event);
}

//--------------------------------------------------------------------------------
// wxGISGPMultiParameter
//--------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISGPMultiParameter, wxGISGPParameter)


wxGISGPMultiParameter::wxGISGPMultiParameter() : wxGISGPParameter()
{
}

wxGISGPMultiParameter::wxGISGPMultiParameter(const wxString &sName, const wxString &sDisplayName, wxGISEnumGPParameterType eType, wxGISEnumGPParameterDataType eDataType) : wxGISGPParameter(sName, sDisplayName, eType, eDataType)
{
}

wxGISGPMultiParameter::~wxGISGPMultiParameter(void)
{
	Clear();
}

void wxGISGPMultiParameter::AddColumn(const wxString &sName)
{
	size_t nRowCount = GetRowCount();
	m_saColumnNames.Add(sName);
	size_t nColCount = GetColumnCount();
	for(size_t i = 0; i < nRowCount; ++i)
	{
		size_t nPos = i + nColCount;
		m_paParameters.Insert(NULL, nPos);
		//m_paParameters.insert(m_paParameters.begin() + nPos, NULL);
	}
}

void wxGISGPMultiParameter::RemoveColumn(size_t nIndex)
{
	size_t nOldColCount = GetColumnCount();
	size_t nRowCount = GetRowCount() - 1;
	for(size_t i = nRowCount; i > 0; --i)
	{
		size_t nPos = i * nOldColCount + nIndex;
		m_paParameters.RemoveAt(nPos);//erase(m_paParameters.begin() + nPos);
	}
	m_saColumnNames.RemoveAt(nIndex);
}

size_t wxGISGPMultiParameter::GetColumnCount(void) const
{
	return m_saColumnNames.GetCount();
}

size_t wxGISGPMultiParameter::GetRowCount(void) const
{
	if(GetColumnCount() == 0)
		return 0;
	return m_paParameters.GetCount() / GetColumnCount();
}

void wxGISGPMultiParameter::AddParameter(size_t nColIndex, size_t nRowIndex, wxGISGPParameter* pParam)
{
	wxCHECK_RET(pParam, "the param should be not NULL"); 
	long nPos = nRowIndex * GetColumnCount() + nColIndex;
	if(m_paParameters.GetCount() <= nPos)
	{
		while(m_paParameters.GetCount() <= nPos)
			m_paParameters.Add(NULL);
	}
	m_paParameters[nPos] = pParam;//???
}

wxGISEnumGPParameterDataType wxGISGPMultiParameter::GetDataType(void) const
{
	return enumGISGPParamDTParamArray;
}

wxString wxGISGPMultiParameter::GetAsString(void) const
{
	wxString sOutputStr;
	for(size_t i = 0; i < m_paParameters.GetCount(); ++i)
		sOutputStr += m_paParameters[i]->GetAsString() + MULTI_PARAM_SEPARATOR;
	return sOutputStr;
}

bool wxGISGPMultiParameter::SetFromString(wxString &sParam)
{
	wxArrayString saParams = wxStringTokenize(sParam, MULTI_PARAM_SEPARATOR, wxTOKEN_RET_EMPTY);
	for(size_t i = 0 ; i < saParams.GetCount(); ++i)
	{
		wxGISGPParameter* pParam = new wxGISGPParameter();
		if(pParam->SetFromString(saParams[i]))
			m_paParameters.Add(pParam);
		else
		{
			wxDELETE(pParam);
			return false;
		}
	}
	return true;
}

wxString wxGISGPMultiParameter::GetColumnName(size_t nIndex) const
{
	wxASSERT_MSG(nIndex < m_saColumnNames.size(), wxT("Column name index is greate than column count!"));
	return m_saColumnNames[nIndex];
}

bool wxGISGPMultiParameter::IsValid(void) const
{
	for(size_t i = 0; i < m_paParameters.GetCount(); ++i)
		if(!m_paParameters[i]->IsValid())
			return false;
	return true;
}

void wxGISGPMultiParameter::SetValid(bool bIsValid)
{
	for(size_t i = 0; i < m_paParameters.GetCount(); ++i)
		m_paParameters[i]->SetValid(bIsValid);
}

wxGISGPParameter* wxGISGPMultiParameter::GetParameter(size_t nCol, size_t nRow) const
{
	long nPos = nRow * GetColumnCount() + nCol;
	return m_paParameters[nPos];
}

void wxGISGPMultiParameter::Clear()
{
    for(size_t i = 0; i < m_paParameters.GetCount(); ++i)
        wxDELETE(m_paParameters[i]);
    m_paParameters.Clear();
}
