/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 NextGIS
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

#include "wxgis/catalogui/ngwpropertypage.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/metadata_16.xpm"

#include <wx/valgen.h>
#include <wx/valtext.h>


#include "wxgis/core/json/jsonwriter.h"

//--------------------------------------------------------------------------
// wxGISNGWResourcePropertyPage
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISNGWResourcePropertyPage, wxGxPropertyPage)

BEGIN_EVENT_TABLE(wxGISNGWResourcePropertyPage, wxGxPropertyPage)
	EVT_TEXT(wxID_ANY, wxGISNGWResourcePropertyPage::OnTextChange)
END_EVENT_TABLE()

wxGISNGWResourcePropertyPage::wxGISNGWResourcePropertyPage(void) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("NGW Resource"));
	//m_PageIcon = wxBitmap(sr_16_xpm);
	m_bHasEdits = false;
}

wxGISNGWResourcePropertyPage::wxGISNGWResourcePropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("NGW Resource"));
	//m_PageIcon = wxBitmap(sr_16_xpm);
	m_bHasEdits = false;
	
    Create(pTrackCancel, parent, id, pos, size, style, name);
}

wxGISNGWResourcePropertyPage::~wxGISNGWResourcePropertyPage()
{
}

void wxGISNGWResourcePropertyPage::Apply(void)
{
	if(!m_bHasEdits)
		return;
		
	if ( Validate() && TransferDataFromWindow() )
	{
		for ( size_t i = 0; i < m_paNGWResources.size(); ++i ) 
		{    
			try
			{
				if(m_paNGWResources.size() == 1)
				{
					m_paNGWResources[i]->UpdateResource(m_sName, m_sKey, m_sDesc);
				}
				else
				{
					m_paNGWResources[i]->UpdateResourceDescritpion(m_sDesc);
				}			
			}
			catch(...)
			{
				
			}
		}
		
		//request updates
		if(!m_paNGWResources.empty())
		{
			wxGxObject* pObj = dynamic_cast<wxGxObject*>(m_paNGWResources[0]);
			if(pObj)
			{
				IGxObjectNotifier* pNotify = dynamic_cast<IGxObjectNotifier*>(pObj);
				if(pNotify)
					pNotify->OnGetUpdates();
			}
		}
	}
	
	m_bHasEdits = false;
}

bool wxGISNGWResourcePropertyPage::CanApply() const
{
	return m_bHasEdits;
}

void wxGISNGWResourcePropertyPage::OnTextChange(wxCommandEvent& event)
{
	m_bHasEdits = true;
}

bool wxGISNGWResourcePropertyPage::CanMerge() const
{
	return true;
}

bool wxGISNGWResourcePropertyPage::FillProperties(wxGxSelection* const pSel)
{
	m_bHasEdits = false;	
	if(NULL == pSel)
		return false;
			
	m_paNGWResources.clear();	
	m_sName = m_sKey = m_sDesc = wxEmptyString;
	
	if(pSel->GetCount() > 1)
	{
		m_Name->Enable(false);
		m_Key->Enable(false);
	}
	else
	{
		m_Name->Enable(true);
		m_Key->Enable(true);
	}
	
	wxGxCatalogBase* pCat = GetGxCatalog();	
	for ( size_t i = 0; i < pSel->GetCount(); ++i ) 
	{    
		wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));	
		if(NULL == pGxObject)
			continue;
		
		wxGxNGWResource* pResource = dynamic_cast<wxGxNGWResource*>(pGxObject);
		if(pResource)
		{
			if(pSel->GetCount() == 1)
			{
				m_sName = pResource->GetResourceName();
				m_sKey = pResource->GetResourceKey();
				m_sDesc = pResource->GetResourceDescription();
			}
			else if(i == 0)
			{
				m_sDesc = pResource->GetResourceDescription();
			}
			
			if(!m_sDesc.IsSameAs(pResource->GetResourceDescription()))
				m_sDesc = wxEmptyString;
				
			m_paNGWResources.push_back(pResource);
		}
	}

	TransferDataToWindow();
	m_bHasEdits = false;	
	return true;
}

bool wxGISNGWResourcePropertyPage::Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;
		
	m_pTrackCancel = pTrackCancel;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText *staticText1 = new wxStaticText( this, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText1->Wrap( -1 );
	bMainSizer->Add( staticText1, 0, wxALL|wxLEFT, 5 );

	m_Name = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sName) );
	bMainSizer->Add( m_Name, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticText *staticText2 = new wxStaticText( this, wxID_ANY, _("Key"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText2->Wrap( -1 );
	bMainSizer->Add( staticText2, 0, wxALL|wxLEFT, 5 );

	wxTextValidator validator(wxFILTER_EMPTY | wxFILTER_INCLUDE_CHAR_LIST, &m_sKey);
	validator.SetCharIncludes(wxT("_-0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	m_Key = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
	bMainSizer->Add( m_Key, 0, wxALL|wxEXPAND, 5 );

    wxStaticText *staticText3 = new wxStaticText( this, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText3->Wrap( -1 );
	bMainSizer->Add( staticText3, 0, wxALL|wxLEFT, 5 );
	
	m_Desc = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE| wxTE_AUTO_URL, wxGenericValidator(&m_sDesc) );
	bMainSizer->Add( m_Desc, 1, wxALL|wxEXPAND, 5 );

	this->SetSizerAndFit( bMainSizer );
	this->Layout();

    return true;
}


//--------------------------------------------------------------------------
// wxGISNGWMetaPropertyPage
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISNGWMetaPropertyPage, wxGxPropertyPage)

BEGIN_EVENT_TABLE(wxGISNGWMetaPropertyPage, wxGxPropertyPage)
	EVT_CHILD_FOCUS( wxGISNGWMetaPropertyPage::OnChildFocus )
	EVT_PG_CHANGED(wxID_ANY, wxGISNGWMetaPropertyPage::OnChanged)
END_EVENT_TABLE()

wxGISNGWMetaPropertyPage::wxGISNGWMetaPropertyPage(void) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("NGW Metadata"));
	m_PageIcon = wxBitmap(metadata_16_xpm);
	m_bHasEdits = false;
	m_pg = NULL;
}

wxGISNGWMetaPropertyPage::wxGISNGWMetaPropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("NGW Metadata"));
	m_PageIcon = wxBitmap(metadata_16_xpm);
	m_bHasEdits = false;
	m_pg = NULL;
	
    Create(pTrackCancel, parent, id, pos, size, style, name);
}

wxGISNGWMetaPropertyPage::~wxGISNGWMetaPropertyPage()
{
}

void wxGISNGWMetaPropertyPage::Apply(void)
{	
	if(!m_bHasEdits)
		return;
		
	wxJSONValue newMetadata;
	//create update of metadata in json format
	for ( wxPropertyGridIterator it = m_pg->GetIterator(); !it.AtEnd(); it++ ) 
	{
		wxPGProperty* p = *it;
		if(p->HasFlag(wxPG_PROP_MODIFIED))
		{
			p->SetModifiedStatus(false);
			m_pg->RefreshProperty(p);
			wxVariant val = p->GetValue();
			wxString sName;
			if(p->IsRoot())
			{
				sName = p->GetName();
			}
			else
			{
				const wxPGProperty* pr = p;
				while(pr && !pr->IsRoot())
				{
					sName.Prepend(pr->GetLabel());
					sName.Prepend(wxT("."));
					pr = pr->GetParent();
				}
				
				if(sName.StartsWith(wxT(".Main.")))
					sName = sName.Right(sName.Len() - 6 );
				else
					sName = sName.Right(sName.Len() - 1 );//remove leading dot				
			}
			
			if(p->IsKindOf(wxCLASSINFO(wxStringProperty)) || p->IsKindOf(wxCLASSINFO(wxArrayStringProperty)) )
				newMetadata[sName] = val.GetString();
			else if( p->IsKindOf(wxCLASSINFO(wxEnumProperty)) || p->IsKindOf(wxCLASSINFO(wxEditEnumProperty)) )	
				newMetadata[sName] = p->GetChoices().GetLabel(p->GetChoiceSelection());
			else if	(p->IsKindOf(wxCLASSINFO(wxFloatProperty)))
				newMetadata[sName] = val.GetDouble();
			else if	(p->IsKindOf(wxCLASSINFO(wxIntProperty)) || p->IsKindOf(wxCLASSINFO(wxUIntProperty)))
				newMetadata[sName] = val.GetLong();
		}
	}
	
	//update metadata

	for ( size_t i = 0; i < m_paNGWResources.size(); ++i ) 
	{    
		try
		{
			m_paNGWResources[i]->UpdateResourceMetadata(newMetadata);
		}
		catch(...)
		{
			//do nothing, try next resource
		}
	}
	
	m_bHasEdits = false;
}


bool wxGISNGWMetaPropertyPage::CanApply() const
{
	return m_bHasEdits;
}

void wxGISNGWMetaPropertyPage::OnChanged(wxPropertyGridEvent& event)
{
	m_bHasEdits = true;
}

bool wxGISNGWMetaPropertyPage::CanMerge() const
{
	return true;
}

bool wxGISNGWMetaPropertyPage::FillProperties(wxGxSelection* const pSel)
{
	m_paNGWResources.clear();	
	m_bHasEdits = false;
	if(m_pg)
	{
		m_pg->Clear();
			
		if(NULL == pSel || pSel->GetCount() == 0)
			return false;
			
		wxJSONValue oFullMetatdata;		
		wxGxNGWService* pService = NULL;
			
		wxGxCatalogBase* pCat = GetGxCatalog();	
		for ( size_t i = 0; i < pSel->GetCount(); ++i ) 
		{    
			wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));	
			if(NULL == pGxObject)
				continue;
			
			wxGxNGWResource* pResource = dynamic_cast<wxGxNGWResource*>(pGxObject);
			if(pResource)
			{
				if(i == 0)
				{
					oFullMetatdata = pResource->GetMetadata();
					pService = pResource->GetNGWService();
					
						/*wxJSONWriter wr;
						wxString sTest;
						wr.Write(oFullMetatdata, sTest);*/
				}
				else
				{
					//compare items and leave only exist in both json arrays
					wxJSONValue metadata = pResource->GetMetadata();
					wxArrayString saKeys = metadata.GetMemberNames();
					
					//remove from metadata items not in full metadata
					for ( size_t j = 0; j < saKeys.GetCount(); ++j ) 
					{    
						wxString sKey = saKeys[j];
						if(oFullMetatdata.HasMember(sKey))
						{
							if(!oFullMetatdata[sKey].IsSameAs(metadata[sKey]))
							{
								switch(oFullMetatdata[sKey].GetType())
								{
									case wxJSONTYPE_STRING:									
										oFullMetatdata[sKey] = wxEmptyString;
										break;
									case wxJSONTYPE_DOUBLE:
										oFullMetatdata[sKey] = 0.0;
										break;
									case wxJSONTYPE_INT:
									case wxJSONTYPE_UINT:
									case wxJSONTYPE_LONG:
									case wxJSONTYPE_INT64:
									case wxJSONTYPE_ULONG:
									case wxJSONTYPE_UINT64:
									case wxJSONTYPE_SHORT:
									case wxJSONTYPE_USHORT:
										oFullMetatdata[sKey] = 0;
										break;
								};								
							}
						}
						else
						{
							saKeys.RemoveAt(j--);
						}
					}
					
					//remove from full metadata items not in metadata
					saKeys = oFullMetatdata.GetMemberNames();
					for ( size_t j = 0; j < saKeys.GetCount(); ++j ) 
					{    
						if(!metadata.HasMember(saKeys[j]))
							oFullMetatdata.Remove(saKeys[j]);
					}
				}
					
				m_paNGWResources.push_back(pResource);
			}
		}
		
		//check if custom meta is present
		if(pService)
		{
			const wxVector<wxGxNGWService::CUSTOM_METADATA_ITEM> & staCustomMeta = pService->GetCustomMetadata();
			for ( size_t i = 0; i < staCustomMeta.size(); ++i ) 
			{    
				if(oFullMetatdata.HasMember(wxT("Custom.") + staCustomMeta[i].sName))
				{
					if(staCustomMeta[i].sType == wxT("wxArrayStringProperty"))
					{
						wxArrayString sValues = wxStringTokenize(staCustomMeta[i].sDefaultValue, wxT(","), wxTOKEN_RET_EMPTY);
						wxString sOrigValue = oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName].AsString();
						oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName][0] = sOrigValue;
						int nCounter = 1;
						for ( size_t j = 0; j < sValues.GetCount(); ++j ) 
						{    
							wxString sVal = sValues[j].Trim(true).Trim(false);
							if(sOrigValue == sVal)
								continue;
							oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName][nCounter++] = sVal;
						}
					}
				}
				else
				{
					if(staCustomMeta[i].sType == wxT("wxArrayStringProperty"))
					{
						wxArrayString sValues = wxStringTokenize(staCustomMeta[i].sDefaultValue, wxT(","), wxTOKEN_RET_EMPTY);
						for ( size_t j = 0; j < sValues.GetCount(); ++j ) 
						{    
							wxString sVal = sValues[j].Trim(true).Trim(false);
							oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName][j] = sVal;
						}
					}	
					else if(staCustomMeta[i].sType == wxT("wxIntProperty"))
						oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName] = wxAtoi(staCustomMeta[i].sDefaultValue);
					else if(staCustomMeta[i].sType == wxT("wxFloatProperty"))
						oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName] = wxAtof(staCustomMeta[i].sDefaultValue);
					else	
						oFullMetatdata[wxT("Custom.") + staCustomMeta[i].sName] = staCustomMeta[i].sDefaultValue;
				}
			}			
		}
		
		FillGrid(oFullMetatdata);
	}
	
	m_bHasEdits = false;
	return true;
}

bool wxGISNGWMetaPropertyPage::Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;
		
	m_pTrackCancel = pTrackCancel;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED | wxPG_AUTO_SORT);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizerAndFit( bMainSizer );
	this->Layout();

    return true;
}

void wxGISNGWMetaPropertyPage::FillGrid(const wxJSONValue& metadata)
{
	wxArrayString saKeys = metadata.GetMemberNames();
	for ( size_t i = 0; i < saKeys.GetCount(); ++i ) //wxEnumProperty wxEditEnumProperty  wxArrayStringProperty 
	{    
		wxString sResultName;
		wxString sName = saKeys[i];
		if(!sName.StartsWith(wxT("Custom.")))
			sName.Prepend(wxT("Main."));
		wxPGProperty* pProp = GetSubProperty(m_pg->GetRoot(), sName, sResultName);
		switch(metadata[saKeys[i]].GetType())
		{
			case wxJSONTYPE_STRING:									
				AppendProperty(pProp, new wxStringProperty(sResultName, wxPG_LABEL, metadata[saKeys[i]].AsString()));
				break;
			case wxJSONTYPE_DOUBLE:
				AppendProperty(pProp, new wxFloatProperty(sResultName, wxPG_LABEL, metadata[saKeys[i]].AsDouble()));
				break;
			case wxJSONTYPE_LONG:
			case wxJSONTYPE_INT64:			
			case wxJSONTYPE_INT:
			case wxJSONTYPE_SHORT:
				AppendProperty(pProp, new wxIntProperty(sResultName, wxPG_LABEL, metadata[saKeys[i]].AsInt()));
				break;
			case wxJSONTYPE_UINT:
			case wxJSONTYPE_ULONG:
			case wxJSONTYPE_UINT64:				
			case wxJSONTYPE_USHORT:
				AppendProperty(pProp, new wxUIntProperty(sResultName, wxPG_LABEL, metadata[saKeys[i]].AsUInt()));
				break;		
			case wxJSONTYPE_ARRAY:
			{
				wxArrayString saValues;
				for ( size_t j = 0; j < metadata[saKeys[i]].Size(); ++j ) 
				{    
					saValues.Add(metadata[saKeys[i]][j].AsString());
				}
				AppendProperty(pProp, new wxEnumProperty(sResultName, wxPG_LABEL, saValues));
			}
				break;
		};		
	}
}

wxPGProperty* wxGISNGWMetaPropertyPage::GetSubProperty(wxPGProperty* pid, const wxString &sName, wxString &sResultName)
{
    wxPGProperty* pRetProperty = pid;
    int nPos = sName.Find('.');
    if(nPos != wxNOT_FOUND)
    {
        wxString sName1 = sName.Left(nPos);
        wxString sName2 = sName.Right(sName.Len() - nPos - 1);
        pRetProperty = m_pg->GetPropertyByName(sName1);
        if(!pRetProperty)
        {
            pRetProperty = AppendProperty(pid, new wxPropertyCategory(sName1) );
        }
        if(sName2.Find('.') != wxNOT_FOUND)
            return GetSubProperty(pRetProperty, sName2, sResultName);
        else
            sResultName = sName2;
    }
    else
        sResultName = sName;
    return pRetProperty;
}

wxPGProperty* wxGISNGWMetaPropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    return pNewProp;
}

void wxGISNGWMetaPropertyPage::OnChildFocus( wxChildFocusEvent& event )
{
	// do nothing to avoid "scrollbar jump" if wx2.9 is used
}
