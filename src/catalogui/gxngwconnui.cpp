/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection UI classes.
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

#include "wxgis/catalogui/gxngwconnui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/processing.h"
#include "wxgis/framework/applicationbase.h"

#include "../../art/pg_vec_16.xpm"
#include "../../art/pg_vec_48.xpm"
#include "../../art/table_pg_16.xpm"
#include "../../art/table_pg_48.xpm"
#include "../../art/layers_16.xpm"
#include "../../art/layer_16.xpm"
#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"
#include "../../art/ngw_layer_16.xpm"
#include "../../art/ngw_layer_48.xpm"
#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"
#include "../../art/rdb_disconn_16.xpm"
#include "../../art/rdb_disconn_48.xpm"
#include "../../art/properties.xpm"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "wx/busyinfo.h"
#include "wx/utils.h"
#include "wx/propdlg.h"
#include "wx/bookctrl.h"

#ifdef wxGIS_USE_CURL


#include "wxgis/catalogui/remoteconndlgs.h"

//--------------------------------------------------------------
//class wxGxNGWServiceUI
//--------------------------------------------------------------

class wxGxNGWLayerUI;
IMPLEMENT_CLASS(wxGxNGWServiceUI, wxGxNGWService)

wxGxNGWServiceUI::wxGxNGWServiceUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon, const wxIcon &icLargeIconDsbl, const wxIcon &icSmallIconDsbl, const wxIcon &icLargeIconGuest, const wxIcon &icSmallIconGuest) : wxGxNGWService(oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
    m_icLargeIconDsbl = icLargeIconDsbl;
    m_icSmallIconDsbl = icSmallIconDsbl;
    m_icLargeIconGuest = icLargeIconGuest;
    m_icSmallIconGuest = icSmallIconGuest;
}

wxGxNGWServiceUI::~wxGxNGWServiceUI(void)
{
}

wxIcon wxGxNGWServiceUI::GetLargeImage(void)
{
    if (IsConnected())
    {
		if(m_bIsAuthorized)
		{
			return m_icLargeIcon;
		}
		else
		{
			return m_icLargeIconGuest;
		}
    }
    else
    {
        return m_icLargeIconDsbl;
    }
}

wxIcon wxGxNGWServiceUI::GetSmallImage(void)
{
    if (IsConnected())
    {
		if(m_bIsAuthorized)
		{
			return m_icSmallIcon;			
		}
		else
		{
			return m_icSmallIconGuest;	
		}
    }
    else
    {
        return m_icSmallIconDsbl;
    }
}

void wxGxNGWServiceUI::EditProperties(wxWindow *parent)
{
	wxGISNGWConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
        Disconnect();
        //reread settings from connection file
        ReadConnectionFile();
	}	
}

bool wxGxNGWServiceUI::Invoke(wxWindow* pParentWnd)
{
    wxBusyCursor wait;
    //connect
	if(!Connect())
	{
		wxGISErrorMessageBox(_("Connect failed!"));
		return false;
	}

    return true;
}

void wxGxNGWServiceUI::LoadChildren(void)
{
    if (m_bChildrenLoaded || !m_bIsConnected)
        return;

    new wxGxNGWRootResourceUI(this, this, _("Resources"), CPLString(m_sURL.ToUTF8()), wxNullIcon, wxIcon(layers_16_xpm));
/*    if(m_bIsAuthorized)
        new wxGxNGWRootUI(this, _("Administration"), CPLString(m_sURL.ToUTF8()), wxNullIcon, wxNullIcon, wxNullIcon, wxNullIcon);
        */
    m_bChildrenLoaded = true;
}

//--------------------------------------------------------------
//class wxGxNGWRootResourceUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWRootResourceUI, wxGxNGWResourceGroupUI)

wxGxNGWRootResourceUI::wxGxNGWRootResourceUI(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWResourceGroupUI(pService, wxJSONValue(), oParent, soName, soPath, icLargeIcon, icSmallIcon)
{
    m_nRemoteId = 0;
    m_sName = wxString(_("Resources"));
	m_sPath = CPLFormFilename(soPath, "resources", "");
}

wxGxNGWRootResourceUI::~wxGxNGWRootResourceUI(void)
{
}

wxIcon wxGxNGWRootResourceUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWRootResourceUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

//--------------------------------------------------------------
//class wxGxNGWResourceGroupUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWResourceGroupUI, wxGxNGWResourceGroup)

wxGxNGWResourceGroupUI::wxGxNGWResourceGroupUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWResourceGroup(pService, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWResourceGroupUI::~wxGxNGWResourceGroupUI(void)
{
}

wxIcon wxGxNGWResourceGroupUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWResourceGroupUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

wxGxObject* wxGxNGWResourceGroupUI::AddResource(const wxJSONValue &Data)
{
    wxGISEnumNGWResourcesType eType = GetType(Data);
	wxGxObject* pReturnObj(NULL);
	
    switch(eType)
    {
    case enumNGWResourceTypeResourceGroup:
		if(!m_icFolderLargeIcon.IsOk())
			m_icFolderLargeIcon = wxIcon(folder_arch_48_xpm);
 		if(!m_icFolderSmallIcon.IsOk())
			m_icFolderSmallIcon = wxIcon(folder_arch_16_xpm);
        pReturnObj = wxDynamicCast(new wxGxNGWResourceGroupUI(m_pService, Data, this, wxEmptyString, m_sPath, m_icFolderLargeIcon, m_icFolderSmallIcon), wxGxObject);
        break;
	case enumNGWResourceTypePostgisLayer:
		if(!m_icPGLayerLargeIcon.IsOk())
			m_icPGLayerLargeIcon = wxIcon(pg_vec_48_xpm);
 		if(!m_icPGLayerSmallIcon.IsOk())
			m_icPGLayerSmallIcon = wxIcon(pg_vec_16_xpm);
        if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayerUI(m_pService, enumNGWResourceTypePostgisLayer, Data, this, wxEmptyString, m_sPath, m_icPGLayerLargeIcon, m_icPGLayerSmallIcon), wxGxObject);
		break;
	case enumNGWResourceTypePostgisConnection:
		if(!m_icPGConnLargeIcon.IsOk())
			m_icPGConnLargeIcon = wxIcon(rdb_conn_48_xpm);
 		if(!m_icPGConnSmallIcon.IsOk())
			m_icPGConnSmallIcon = wxIcon(rdb_conn_16_xpm);
		if(!m_icPGDisConnLargeIcon.IsOk())
			m_icPGDisConnLargeIcon = wxIcon(rdb_disconn_48_xpm);
 		if(!m_icPGDisConnSmallIcon.IsOk())
			m_icPGDisConnSmallIcon = wxIcon(rdb_disconn_16_xpm);
		if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWPostGISConnectionUI(m_pService, Data, this, wxEmptyString, m_sPath, m_icPGConnLargeIcon, m_icPGConnSmallIcon, m_icPGDisConnLargeIcon, m_icPGDisConnSmallIcon), wxGxObject);
		break;
	case enumNGWResourceTypeWMSServerService:
		break;
	case enumNGWResourceTypeBaseLayers:
		break;
	case enumNGWResourceTypeWebMap:
		break;
	case enumNGWResourceTypeWFSServerService:
		break;
	case enumNGWResourceTypeVectorLayer:
		if(!m_icNGWLayerLargeIcon.IsOk())
			m_icNGWLayerLargeIcon = wxIcon(ngw_layer_48_xpm);
 		if(!m_icNGWLayerSmallIcon.IsOk())
			m_icNGWLayerSmallIcon = wxIcon(ngw_layer_16_xpm);
        if(m_bHasGeoJSON)
			pReturnObj = wxDynamicCast(new wxGxNGWLayerUI(m_pService, enumNGWResourceTypeVectorLayer, Data, this, wxEmptyString, m_sPath, m_icNGWLayerLargeIcon, m_icNGWLayerSmallIcon), wxGxObject);
		break;
    }
	
	return pReturnObj;	
}

bool wxGxNGWResourceGroupUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if (NULL == pCatalog)
    {
        return false;
    }

    wxWindow* pWnd = dynamic_cast<wxWindow*>(GetApplication());
    wxGxObjectFilter* pFilter = new wxGxDatasetFilter(enumGISRasterDataset, enumRasterPostGIS);

    wxBusyCursor wait;

    for (size_t i = 0; i < saGxObjectPaths.GetCount(); ++i)
    {
        wxGxObject* pGxObject = pCatalog->FindGxObject(saGxObjectPaths[i]);
        if (NULL != pGxObject)
        {
			wxGxNGWResource* pNGWResource = dynamic_cast<wxGxNGWResource*>(pGxObject);
            if (NULL != pNGWResource)
            {
				if(bMove) //TODO: check if this same NGW
				{
					
				}
				else //TODO: check if this is not same NGW
				{
					
				}
			}
			else if(pGxObject->IsKindOf(wxCLASSINFO(wxGxFeatureDataset)))
			{
				
			}
		}
	}
	
    return true;
}

void wxGxNGWResourceGroupUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());
	
	//TODO: add NGW property page
	
	//TODO: add permissions property page


    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

//--------------------------------------------------------------
//class wxGxNGWLayerUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWLayerUI, wxGxNGWLayer)

wxGxNGWLayerUI::wxGxNGWLayerUI(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon) : wxGxNGWLayer(pService, eType, Data, oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
}

wxGxNGWLayerUI::~wxGxNGWLayerUI(void)
{
}

wxIcon wxGxNGWLayerUI::GetLargeImage(void)
{
    return m_icLargeIcon;
}

wxIcon wxGxNGWLayerUI::GetSmallImage(void)
{
    return m_icSmallIcon;
}

void wxGxNGWLayerUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());
	
	//TODO: add NGW property page

    wxGISVectorPropertyPage* VectorPropertyPage = new wxGISVectorPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(VectorPropertyPage, VectorPropertyPage->GetPageName());
	
	//TODO: add fields property page

	wxGISDataset* pDset = GetDataset();
	if(pDset)
	{
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());
        wsDELETE(pDset);
	}

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

//--------------------------------------------------------------
//class wxGxNGWPostGISConnectionUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWPostGISConnectionUI, wxGxRemoteConnectionUI)

wxGxNGWPostGISConnectionUI::wxGxNGWPostGISConnectionUI(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIconConn, const wxIcon &SmallIconConn, const wxIcon &LargeIconDisconn, const wxIcon &SmallIconDisconn) : wxGxRemoteConnectionUI(oParent, soName, soPath, LargeIconConn, SmallIconConn, LargeIconDisconn, SmallIconDisconn), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypePostgisConnection;
    m_pService = pService;
    m_sName = m_sDisplayName;
	m_sPath = CPLFormFilename(soPath, m_sName.ToUTF8(), "");
	
	wxJSONValue JSONConn = Data[wxT("postgis_connection")];
	m_sUser =  JSONConn[wxT("username")].AsString();
	m_sPass =  JSONConn[wxT("password")].AsString();
	m_sDatabase =  JSONConn[wxT("database")].AsString();
	m_sHost =  JSONConn[wxT("hostname")].AsString();
}

wxGxNGWPostGISConnectionUI::~wxGxNGWPostGISConnectionUI(void)
{
}

wxString wxGxNGWPostGISConnectionUI::GetCategory(void) const
{ 
	return wxString(_("NGW PostGIS Connection")); 
}

wxGISDataset* const wxGxNGWPostGISConnectionUI::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sUser, m_sPass, wxT("5432"), m_sHost, m_sDatabase);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

int wxGxNGWPostGISConnectionUI::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetRemoteId();
}


bool wxGxNGWPostGISConnectionUI::CanDelete(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnectionUI::CanRename(void)
{
    //TODO: check permissions
    return m_pService != NULL;
}

bool wxGxNGWPostGISConnectionUI::Delete(void)
{
    if( DeleteResource() )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnectionUI::Rename(const wxString &sNewName)
{
    if( RenameResource(sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxNGWPostGISConnectionUI::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWPostGISConnectionUI::CanCopy(const CPLString &szDestPath)
{
    return false;
}

bool wxGxNGWPostGISConnectionUI::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxNGWPostGISConnectionUI::CanMove(const CPLString &szDestPath)
{
    return false;
}

void wxGxNGWPostGISConnectionUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());
	
	//TODO: add NGW property page

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

#endif // wxGIS_USE_CURL
