/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  parameter class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/geoprocessing/gpdomain.h"

#include "wx/event.h"

class WXDLLIMPEXP_GIS_GP wxGISGPParamEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGPPARAM_CHANGED, wxGISGPParamEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGPPARAM_MSG_SET, wxGISGPParamEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGPPARAM_DOMAIN_ADDVAL, wxGISGPParamEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGPPARAM_DOMAIN_CLEAR, wxGISGPParamEvent);

/** \class wxGISGPParamEvent gpparam.h
    \brief The wxGISProcess class event.
*/
class WXDLLIMPEXP_GIS_GP wxGISGPParamEvent : public wxEvent
{
public:
    wxGISGPParamEvent(int nId = wxID_ANY, wxEventType eventType = wxGPPARAM_CHANGED, const wxVariant &Value = wxNullVariant, const wxString &sName = wxEmptyString ) : wxEvent(nId, eventType)
	{		
		m_Value = Value;
		m_sName = sName;
	}

    wxGISGPParamEvent(const wxGISGPParamEvent& event) : wxEvent(event)
	{
		m_Value = event.m_Value;
		m_sName = event.m_sName;
	}

    wxVariant GetParamValue() const { return m_Value; }
    void SetParamValue(const wxVariant &Value) { m_Value = Value; }
    wxString GetName() const { return m_sName; }
    void SetName(const wxString &sName) { m_sName = sName; }

    virtual wxEvent *Clone() const { return new wxGISGPParamEvent(*this); }

protected:
    wxVariant m_Value;
    wxString m_sName;
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISGPParamEvent)
};

typedef void (wxEvtHandler::*wxGISGPParamEventFunction)(wxGISGPParamEvent&);

#define wxGISGPParamEventHandler(func) \
	wxEVENT_HANDLER_CAST(wxGISGPParamEventFunction, func)

#define EVT_GPPARAM_CHANGED(func)  wx__DECLARE_EVT0(wxGPPARAM_CHANGED, wxGISGPParamEventHandler(func))
#define EVT_GPPARAM_MSG_SET(func)  wx__DECLARE_EVT0(wxGPPARAM_MSG_SET, wxGISGPParamEventHandler(func))
#define EVT_GPPARAM_DOMAIN_ADDVAL(func)  wx__DECLARE_EVT0(wxGPPARAM_DOMAIN_ADDVAL, wxGISGPParamEventHandler(func))
#define EVT_GPPARAM_DOMAIN_CLEAR(func)  wx__DECLARE_EVT0(wxGPPARAM_DOMAIN_CLEAR, wxGISGPParamEventHandler(func))

/** \class wxGISGPParameter gpparam.h
 *  \brief A Geoprocessing tools parameter class.
 */
class WXDLLIMPEXP_GIS_GP wxGISGPParameter : 
    public wxEvtHandler,
    public wxGISConnectionPointContainer,
    public IGISGPDomainParent
{
    friend class wxGISGPParameterArray;
    DECLARE_CLASS(wxGISGPParameter)
public:
    wxGISGPParameter(void);
    wxGISGPParameter(const wxString &sName, const wxString &sDisplayName, wxGISEnumGPParameterType eType, wxGISEnumGPParameterDataType eDataType);
    virtual ~wxGISGPParameter(void);
    virtual bool GetAltered(void) const;
    virtual void SetAltered(bool bAltered);
    virtual bool GetHasBeenValidated(void) const;
    virtual void SetHasBeenValidated(bool bHasBeenValidated);
    virtual bool IsValid(void) const;
    virtual void SetValid(bool bIsValid);
    virtual wxString GetName(void) const;
    virtual void SetName(const wxString &sName);
    virtual wxString GetDisplayName(void) const;
    virtual void SetDisplayName(const wxString &sDisplayName);
    virtual wxGISEnumGPParameterDataType GetDataType(void) const;
    virtual void SetDataType(wxGISEnumGPParameterDataType nType);
    virtual wxGISEnumGPParameterDirection GetDirection(void) const;
    virtual void SetDirection(wxGISEnumGPParameterDirection nDirection);
    virtual wxGISEnumGPParameterType GetParameterType(void) const;
    virtual void SetParameterType(wxGISEnumGPParameterType nType);
    virtual wxVariant GetValue(void) const;
    virtual void SetValue(const wxVariant &Val);
    virtual wxGISGPValueDomain* GetDomain(void) const;
    virtual void SetDomain(wxGISGPValueDomain* pDomain);
    virtual wxString GetMessage(void) const;
    virtual wxGISEnumGPMessageType GetMessageType(void) const;
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, const wxString &sMsg = wxEmptyString);
    virtual void AddDependency(const wxString &sParamName);
    virtual wxArrayString GetDependences(void) const;
    /** \fn wxString GetAsString(void)
     *  \brief Serialize parameter to string.
     *  \return The string representation of parameter
     */	
	virtual wxString GetAsString(void) const;
    /** \fn void SetFromString(wxString)
     *  \brief Serialize parameter from string.
     *  \param sParam The string representation of parameter
     */	
    virtual bool SetFromString(wxString &sParam);
	virtual int GetSelDomainValue(void) const;
	virtual void SetSelDomainValue(int nNewSelection);
    //
    virtual void OnValueAdded(const wxVariant &Value, const wxString &sName);
    virtual void OnCleared(void);
protected:
    virtual void SetId(size_t nId);
    virtual size_t GetId(void) const;
protected:
    bool m_bAltered;
    bool m_bHasBeenValidated;
    bool m_bIsValid;
    wxString m_sName;
    wxString m_sDisplayName;
    wxGISEnumGPParameterDataType m_DataType;
    wxGISEnumGPParameterDirection m_Direction;
    wxGISEnumGPParameterType m_ParameterType;
    wxVariant m_Value;
    wxGISGPValueDomain* m_pDomain;
    wxString m_sMessage;
    wxGISEnumGPMessageType m_nMsgType;
	int m_nSelection;
    wxArrayString m_saDependencies;
    size_t m_nId;
};

/** \typedef GPParameters
 *  \brief The parameters array.
 */
//WX_DECLARE_USER_EXPORTED_BASEARRAY(wxGISGPParameter *, wxGISGPParamArray, WXDLLIMPEXP_GIS_GP);
WX_DEFINE_USER_EXPORTED_ARRAY(wxGISGPParameter*, wxGISGPParamArray, class WXDLLIMPEXP_GIS_GP);

class WXDLLIMPEXP_GIS_GP wxGISGPParameterArray : public wxGISGPParamArray
{
public:    
    void Add (wxGISGPParameter * item)
    {
        wxGISGPParamArray::Add(item, 1);
        item->SetId(GetCount() - 1);
    }
    wxGISGPParameter * GetParameter(const wxString &sParamName) const
    {
        for(size_t i = 0; i < GetCount(); ++i)
            if(Item(i)->GetName().IsSameAs(sParamName, false))
                return Item(i);
        return NULL;
    }
};


/** \class wxGISGPMultiParameter gpparam.h
 *  \brief A Geoprocessing tools multiparameter class.
 *
 *  This class store parameters in array. In tool dialog it show the grid filled with parmeter data.
 */
class WXDLLIMPEXP_GIS_GP wxGISGPMultiParameter : public wxGISGPParameter
{
    DECLARE_CLASS(wxGISGPMultiParameter)
public:
	wxGISGPMultiParameter(void);
    wxGISGPMultiParameter(const wxString &sName, const wxString &sDisplayName, wxGISEnumGPParameterType eType, wxGISEnumGPParameterDataType eDataType);
    virtual ~wxGISGPMultiParameter(void);
	virtual void AddColumn(const wxString &sName);
	virtual void RemoveColumn(size_t nIndex);
	virtual size_t GetColumnCount(void) const;
	virtual wxString GetColumnName(size_t nIndex) const;
	virtual size_t GetRowCount(void) const;
	virtual void AddParameter(size_t nColIndex, size_t nRowIndex, wxGISGPParameter* pParam);
    virtual bool IsValid(void) const;
    virtual void SetValid(bool bIsValid);
	virtual void Clear();
    //wxGISGPParameter
    virtual wxGISEnumGPParameterDataType GetDataType(void) const;
	virtual wxString GetAsString(void) const;
    virtual bool SetFromString(wxString &sParam);
	virtual wxGISGPParameter* GetParameter(size_t nCol, size_t nRow) const;
protected:
	wxGISGPParameterArray m_paParameters;
	wxArrayString m_saColumnNames;
};
