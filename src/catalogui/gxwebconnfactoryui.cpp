/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnectionFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxwebconnfactoryui.h"
#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/catalogui/gxngwconnui.h"

#include "../../art/web_tms_conn_16.xpm"
#include "../../art/web_tms_conn_48.xpm"
#include "../../art/web_tms_disconn_16.xpm"
#include "../../art/web_tms_disconn_48.xpm"
#include "../../art/ngw_conn_16.xpm"
#include "../../art/ngw_conn_48.xpm"
#include "../../art/ngw_disconn_16.xpm"
#include "../../art/ngw_disconn_48.xpm"

//------------------------------------------------------------------------------
// wxGxWebConnectionFactoryUI
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxWebConnectionFactoryUI, wxGxWebConnectionFactory)

wxGxWebConnectionFactoryUI::wxGxWebConnectionFactoryUI(void) : wxGxWebConnectionFactory()
{

    m_LargeIconConn = wxIcon(web_tms_conn_48_xpm);
    m_SmallIconConn = wxIcon(web_tms_conn_16_xpm);
    m_LargeIconDisconn = wxIcon(web_tms_disconn_48_xpm);
    m_SmallIconDisconn = wxIcon(web_tms_disconn_16_xpm);
    m_LargeIconNGWConn = wxIcon(ngw_conn_48_xpm);
    m_SmallIconNGWConn = wxIcon(ngw_conn_16_xpm);
    m_LargeIconNGWDisconn = wxIcon(ngw_disconn_48_xpm);
    m_SmallIconNGWDisconn = wxIcon(ngw_disconn_16_xpm);
}

wxGxWebConnectionFactoryUI::~wxGxWebConnectionFactoryUI(void)
{
}

wxGxObject* wxGxWebConnectionFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }



    wxXmlDocument config(wxString::FromUTF8(szPath));
    if (config.IsOk())
    {
        wxXmlNode* pRoot = config.GetRoot();
        if (pRoot != NULL)
        {
            if (pRoot->GetName().IsSameAs(wxT("NGW"), false))
            {
                wxGxNGWServiceUI* pDataset = new wxGxNGWServiceUI(pParent, soName, szPath, m_LargeIconNGWConn, m_SmallIconNGWConn, m_LargeIconNGWDisconn, m_SmallIconNGWDisconn);
                return wxStaticCast(pDataset, wxGxObject);
            }
            else if (pRoot->GetName().IsSameAs(wxT("GDAL_WMS"), false))
            {
                wxGxTMSWebServiceUI* pDataset = new wxGxTMSWebServiceUI(pParent, soName, szPath, m_LargeIconConn, m_SmallIconConn, m_LargeIconDisconn, m_SmallIconDisconn);
	            return wxStaticCast(pDataset, wxGxObject);
            }
        }
    }
    return NULL;

}
