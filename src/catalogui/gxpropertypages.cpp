/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2011,2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxpropertypages.h"

//-------------------------------------------------------------------------------
// wxGISCatalogGeneralPropertyPage
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogGeneralPropertyPage, wxPanel)

wxGISCatalogGeneralPropertyPage::wxGISCatalogGeneralPropertyPage(void) : m_pCatalog(NULL)
{
}

wxGISCatalogGeneralPropertyPage::~wxGISCatalogGeneralPropertyPage()
{
}

bool wxGISCatalogGeneralPropertyPage::Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;

	m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
	if(!m_pCatalog)
        return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* sbRootSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("What top level entries do you want the Catalog to contain?") ), wxVERTICAL );
	
    //fill root items
    m_pRootItems = new wxGISCheckList( this, ID_ROOTLISTCTRL);
    //fill code
    wxVector<wxGxCatalog::ROOTITEM>* const m_staRootItems = m_pCatalog->GetRootItems();
    for (size_t i = 0; i < m_staRootItems->size(); ++i)
    {
        wxString sName = m_staRootItems->operator[](i).sName;
        int nPos = m_pRootItems->InsertItem(sName,  m_staRootItems->operator[](i).bEnabled == true ? 1 : 0);
        m_pRootItems->SetItemData(nPos, i);
    }

	sbRootSizer->Add( m_pRootItems, 1, wxALL | wxEXPAND, 5 );

    wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_button3 = new wxButton( this, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, 0 );
    m_button3->Enable(false);
	bSizer1->Add( m_button3, 0, wxALL | wxALIGN_RIGHT, 5 );

	sbRootSizer->Add( bSizer1, 0, wxALL | wxEXPAND, 5 );

	bMainSizer->Add( sbRootSizer, 1, wxALL | wxEXPAND, 5 );

    //===================================================================

    wxStaticBoxSizer* sbFactorySizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Which types of data do you want the Catalog to show?") ), wxVERTICAL );
	
    //fill factories

    m_pFactoryItems = new wxGISCheckList( this, ID_FACTORYLISTCTRL);
    //fill code
    wxVector<wxGxObjectFactory*>* const pFactories = m_pCatalog->GetObjectFactories();
    for(size_t i = 0; i < pFactories->size(); ++i)
    {
        wxGxObjectFactory* pGxObjectFactory = pFactories->operator[](i);
        wxString sName = pGxObjectFactory->GetName();
        long nPos = m_pFactoryItems->InsertItem(sName, pGxObjectFactory->GetEnabled());
        m_pFactoryItems->SetItemData(nPos, (long)pGxObjectFactory);
    }

    sbFactorySizer->Add( m_pFactoryItems, 1, wxALL | wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_button2 = new wxButton( this, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, 0 );
    m_button2->Enable(false);
	bSizer2->Add( m_button2, 0, wxALL | wxALIGN_RIGHT, 5 );
	
	sbFactorySizer->Add( bSizer2, 0, wxALL | wxEXPAND, 5 );
	
	bMainSizer->Add( sbFactorySizer, 1, wxALL | wxEXPAND, 5 );
	
	m_checkBoxHideExt = new wxCheckBox( this, wxID_ANY, _("Hide file extensions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxHideExt->SetValue(!m_pCatalog->GetShowExt());
	
	bMainSizer->Add( m_checkBoxHideExt, 0, wxALL, 5 );
	
	m_checkBoxLast = new wxCheckBox( this, wxID_ANY, _("Return to last location when wxGISCatalog start up"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxLast->SetValue(m_pCatalog->GetOpenLastPath());
	
	bMainSizer->Add( m_checkBoxLast, 0, wxALL, 5 );
	
	m_checkBoxHidden = new wxCheckBox( this, wxID_ANY, _("Show hidden items"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxHidden->SetValue(m_pCatalog->GetShowHidden());
	
	bMainSizer->Add( m_checkBoxHidden, 0, wxALL, 5 );
	
    this->SetSizerAndFit(bMainSizer);
	this->Layout();

    return true;
}

void wxGISCatalogGeneralPropertyPage::Apply(void)
{
    bool bOpenLast = m_checkBoxLast->GetValue();
    bool bHideExt = m_checkBoxHideExt->GetValue();
    bool bShowHidden = m_checkBoxHidden->GetValue();
    bool bHaveChanges = bHideExt == m_pCatalog->GetShowExt() || bOpenLast != m_pCatalog->GetOpenLastPath() || bShowHidden != m_pCatalog->GetShowHidden();

    m_pCatalog->SetShowExt(!bHideExt);
    m_pCatalog->SetShowHidden(bShowHidden);
    m_pCatalog->SetOpenLastPath(bOpenLast);

    //update object factories
    for(int i = 0; i < m_pFactoryItems->GetItemCount(); ++i)
    {
        if(m_pFactoryItems->IsItemChanged(i))
        {
            if(!bHaveChanges)
                bHaveChanges = true;
            wxGxObjectFactory* pGxObjectFactory = (wxGxObjectFactory*)m_pFactoryItems->GetItemData(i);
            pGxObjectFactory->SetEnabled(m_pFactoryItems->GetItemCheckState(i) != 0);
        }
    }

    if(bHaveChanges)
    {
        m_pCatalog->Refresh();
    }

    //update root items
    for(int i = 0; i < m_pRootItems->GetItemCount(); ++i)
    {
        if(m_pRootItems->IsItemChanged(i))
        {
            int nIndex = m_pRootItems->GetItemData(i);
            bool bChecked = m_pRootItems->GetItemCheckState(i) != 0;
            m_pCatalog->EnableRootItem(nIndex, bChecked);
        }
    }
}
