/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTablePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Bishop
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
#include "wxgis/catalogui/tablepropertypage.h"
#include "wxgis/datasource/sysop.h"

#include "wx/propgrid/advprops.h"

//--------------------------------------------------------------------------
// wxGISTablePropertyPage
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISTablePropertyPage, wxPanel)

BEGIN_EVENT_TABLE(wxGISTablePropertyPage, wxPanel)
END_EVENT_TABLE()

wxGISTablePropertyPage::wxGISTablePropertyPage(void)
{
	m_nCounter = 0;
    m_pDataset = NULL;
    m_pGxDataset = NULL;
}

wxGISTablePropertyPage::wxGISTablePropertyPage(wxGxTableDataset* pGxDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_nCounter = 0;
    m_pDataset = NULL;
    m_pGxDataset = NULL;    
    Create(pGxDataset, parent, id, pos, size, style, name);
}

wxGISTablePropertyPage::~wxGISTablePropertyPage()
{
    wsDELETE(m_pDataset);
}

bool wxGISTablePropertyPage::Create(wxGxTableDataset* pGxDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;

    m_pGxDataset = pGxDataset;

    m_pDataset = wxDynamicCast(m_pGxDataset->GetDataset(false), wxGISTable);
    if(!m_pDataset)
        return false;
	if(!m_pDataset->IsOpened())
		if(!m_pDataset->Open(0, 0, false))
			return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    FillGrid();

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

wxPGProperty* wxGISTablePropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISTablePropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISTablePropertyPage::AppendMetadataProperty(wxString sMeta)
{
    int nPos = sMeta.Find('=');
    if(nPos == wxNOT_FOUND)
		return AppendProperty( new wxStringProperty(_("Item"), wxString::Format(wxT("Item_%d"), ++m_nCounter), sMeta) );
    else
    {
        wxString sName = sMeta.Left(nPos);
        wxString sVal = sMeta.Right(sMeta.Len() - nPos - 1);
        //clean
        wxString sCleanVal;
        for(size_t i = 0; i < sVal.Len(); ++i)
        {
            char c = sVal.GetChar(i);
            if(c > 31 && c != 127)
                sCleanVal += c;
        }
        if(sCleanVal.Len() > 500)
            return m_pg->Append( new wxLongStringProperty(sName, wxString::Format(wxT("%s_%d"), sName.c_str(), ++m_nCounter), sCleanVal) );//??
        else
            return AppendProperty( new wxStringProperty(sName, wxString::Format(wxT("%s_%d"), sName.c_str(), ++m_nCounter), sCleanVal) );
    }
}

void wxGISTablePropertyPage::FillGrid(void)
{
    //reset grid
    m_pg->Clear();

    wxString sTmp;
    //fill propertygrid
    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Data Source")) );
    AppendProperty( new wxStringProperty(_("Table"), wxPG_LABEL, m_pDataset->GetName()) );  

    CPLString soPath(m_pDataset->GetPath());

    //folder
	wxString sPath(CPLGetPath(soPath), wxConvUTF8);
    AppendProperty( new wxStringProperty(_("Folder"), wxPG_LABEL, sPath) );  
    
    char** papszFileList = m_pDataset->GetFileList();
    if( !papszFileList || CSLCount(papszFileList) == 0 )
    {
        AppendProperty( new wxStringProperty(_("Files"), wxPG_LABEL, _("None associated")) );  
    }
    else
    {
        wxPGProperty* pfilesid = AppendProperty(pid, new wxPropertyCategory(_("Files")) );  
        for(int i = 0; papszFileList[i] != NULL; ++i )
	    {
		    wxString sFileName = GetConvName(papszFileList[i]);
			AppendProperty(pfilesid, new wxStringProperty(wxString::Format(_("File %d"), i), wxPG_LABEL, sFileName) );  
	    }
    }
    CSLDestroy( papszFileList );
    
    m_pGxDataset->FillMetadata();
    //size    
    AppendProperty(pid, new wxStringProperty(_("Total size"), wxPG_LABEL, wxFileName::GetHumanReadableSize(m_pGxDataset->GetSize())) );
    AppendProperty(pid, new wxDateProperty(_("Modification date"), wxPG_LABEL, m_pGxDataset->GetModificationDate()) );

    OGRDataSource *pDataSource = m_pDataset->GetDataSourceRef();
    if(pDataSource)
    {
        OGRSFDriver* pDrv = pDataSource->GetDriver();
        if(pDrv)
        {
            wxPGProperty* pdriversid = AppendProperty(pid, new wxStringProperty(_("Driver"), wxPG_LABEL, wxString(pDrv->GetName(), wxConvUTF8) ));  
            //TestCapability
            AppendProperty(pdriversid, new wxStringProperty(_("Create DataSource"), wxPG_LABEL, pDrv->TestCapability(ODrCCreateDataSource) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pdriversid, new wxStringProperty(_("Delete DataSource"), wxPG_LABEL, pDrv->TestCapability(ODrCDeleteDataSource) == TRUE ? _("true") : _("false")) );  
        }
        wxPGProperty* pdssid = AppendProperty(pid, new wxStringProperty(_("DataSource"), wxT("DataSource_det"), wxString(pDataSource->GetName(), wxConvUTF8) ));  
        AppendProperty(pdssid, new wxIntProperty(_("Layer Count"), wxPG_LABEL, pDataSource->GetLayerCount()) );  
        AppendProperty(pdssid, new wxStringProperty(_("Create DataSource"), wxPG_LABEL, pDataSource->TestCapability(ODsCCreateLayer) == TRUE ? _("true") : _("false")) );  
    }

    if(pDataSource)
    {
        AppendProperty( new wxPropertyCategory(_("Information")) );
        if(m_pGxDataset->GetType() == enumGISContainer)
        {
            for( int iLayer = 0; iLayer < pDataSource->GetLayerCount(); iLayer++ )
            {
                OGRLayer *poLayer = pDataSource->GetLayer(iLayer);
                FillLayerDef(poLayer, iLayer, soPath);
            }
        }
        else
        {
            OGRLayer *poLayer = m_pDataset->GetLayerRef();
            FillLayerDef(poLayer, 0, soPath);
        }
    }
}

void wxGISTablePropertyPage::FillLayerDef(OGRLayer *poLayer, int iLayer, CPLString soPath)
{
    wxPGProperty* playid = AppendProperty( new wxPropertyCategory(wxString::Format(_("Layer #%d"), iLayer + 1) ));

    wxString sOut = GetConvName(poLayer->GetName(), false);
	if(sOut.IsEmpty())
	{
        sOut = GetConvName(CPLGetBasename(soPath), false);
	}

    AppendProperty(playid, new wxStringProperty(_("Name"), wxPG_LABEL, sOut));  //GetConvName

	AppendProperty(playid, new wxIntProperty(_("Feature count"), wxPG_LABEL, m_pDataset->GetFeatureCount() ));  

    wxString sFidCol = m_pDataset->GetFIDColumn();
    if (!sFidCol.IsEmpty())
        AppendProperty(playid, new wxStringProperty(_("FID Column"), wxPG_LABEL, sFidCol));

    OGRFeatureDefn* const poDefn = poLayer->GetLayerDefn();
    if(poDefn)
    {
        wxPGProperty* pfieldsid = AppendProperty(playid, new wxPropertyCategory(wxString::Format(_("Layer #%d Fields (%d)"), iLayer + 1, poDefn->GetFieldCount())) );
        for( int iAttr = 0; iAttr < poDefn->GetFieldCount(); iAttr++ )
        {
            OGRFieldDefn    *poField = poDefn->GetFieldDefn( iAttr );
            wxString sFieldTypeName = wxString( poField->GetFieldTypeName( poField->GetType() ), wxConvLocal );
			wxPGProperty* pfielid = AppendProperty(pfieldsid, new wxStringProperty(_("Name"), wxString::Format(wxT("Name_%d"), iAttr), wxString::Format(wxT("%s (%s)"), wxString(poField->GetNameRef(), wxConvLocal), sFieldTypeName.c_str()) ));  
            AppendProperty(pfielid, new wxStringProperty(_("Type"), wxString::Format(wxT("Type_%d"), iAttr), sFieldTypeName ) );  
            AppendProperty(pfielid, new wxIntProperty(_("Width"), wxString::Format(wxT("Width_%d"), iAttr), poField->GetWidth()) );  
            AppendProperty(pfielid, new wxIntProperty(_("Precision"), wxString::Format(wxT("Precision_%d"), iAttr), poField->GetPrecision()) ); 
            OGRJustification Just = poField->GetJustify();
            wxString sJust(_("Undefined"));
            switch(Just)
            {
            case OJLeft:
                sJust = wxString(_("Left"));
                break;
            case OJRight:
                sJust = wxString(_("Right"));
                break;
            }
            AppendProperty(pfielid, new wxStringProperty(_("Justify"), wxString::Format(wxT("Justify_%d"), iAttr), sJust) ); 
            m_pg->Collapse(pfielid);

        }
    }
    //TestCapability 
    wxPGProperty* pcapid = AppendProperty(playid, new wxPropertyCategory(wxString::Format(_("Layer #%d Capability"), iLayer + 1)) );
    AppendProperty(pcapid, new wxStringProperty(_("Random Read"), wxPG_LABEL, poLayer->TestCapability(OLCRandomRead) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Sequential Write"), wxPG_LABEL, poLayer->TestCapability(OLCSequentialWrite) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Random Write"), wxPG_LABEL, poLayer->TestCapability(OLCRandomWrite) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Fast Spatial Filter"), wxPG_LABEL, poLayer->TestCapability(OLCFastSpatialFilter) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Fast Feature Count"), wxPG_LABEL, poLayer->TestCapability(OLCFastFeatureCount) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Fast Get Extent"), wxPG_LABEL, poLayer->TestCapability(OLCFastGetExtent) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Fast Set Next By Index"), wxPG_LABEL, poLayer->TestCapability(OLCFastSetNextByIndex) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Create Field"), wxPG_LABEL, poLayer->TestCapability(OLCCreateField) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Delete Feature"), wxPG_LABEL, poLayer->TestCapability(OLCDeleteFeature) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Strings As UTF8"), wxPG_LABEL, poLayer->TestCapability(OLCStringsAsUTF8) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Transactions"), wxPG_LABEL, poLayer->TestCapability(OLCTransactions) == TRUE ? _("true") : _("false")) );  
    AppendProperty(pcapid, new wxStringProperty(_("Ignore Fields"), wxPG_LABEL, poLayer->TestCapability(OLCIgnoreFields) == TRUE ? _("true") : _("false")) ); 
}
