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

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "../../art/properties.xpm"

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

wxGxNGWServiceUI::wxGxNGWServiceUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &icLargeIcon, const wxIcon &icSmallIcon, const wxIcon &icLargeIconDsbl, const wxIcon &icSmallIconDsbl) : wxGxNGWService(oParent, soName, soPath)
{
    m_icLargeIcon = icLargeIcon;
    m_icSmallIcon = icSmallIcon;
    m_icLargeIconDsbl = icLargeIconDsbl;
    m_icSmallIconDsbl = icSmallIconDsbl;
}

wxGxNGWServiceUI::~wxGxNGWServiceUI(void)
{
}

wxIcon wxGxNGWServiceUI::GetLargeImage(void)
{
    if (IsConnected())
    {
        return m_icLargeIcon;
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
        return m_icSmallIcon;
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

wxDragResult wxGxNGWServiceUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxNGWServiceUI::Drop(const wxArrayString& saGxObjectPaths, bool bMove)
{
    return false;
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

#endif // wxGIS_USE_CURL
