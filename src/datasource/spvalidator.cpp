/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferenceValidator class. Return SpatialReference limits
 * Author:   Baryshnikov Dmitry (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/datasource/spvalidator.h"

/*
#include "wxgis/core/config.h"

#include <wx/mstream.h>
#include <wx/xml/xml.h>

#define LIMITS_NAME wxString(wxT("area.xml"))

wxGISSpatialReferenceValidator::wxGISSpatialReferenceValidator(void) : m_bIsOK(false)
{
    m_sPath = wxString(CPLGetConfigOption("wxGxSpatialReferencesFolder",""), wxConvUTF8);
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath + wxT("/") + LIMITS_NAME);
        return;
    }
	wxGISConfig m_Conf(wxString(wxT("wxCatalog")), CONFIG_DIR);

	wxXmlNode* pConfXmlNode = m_Conf.GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		pConfXmlNode = m_Conf.GetConfigNode(enumGISHKLM, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		return;

	//search for path
	wxXmlNode* pRootItemsNode = m_Conf.GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems"))); 
	m_sPath = GetPath(pRootItemsNode);
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath);
        return;
    }
	pRootItemsNode = m_Conf.GetConfigNode(enumGISHKLM, wxString(wxT("catalog/rootitems"))); 
	m_sPath = GetPath(pRootItemsNode);
    if(!m_sPath.IsEmpty())
    {
        m_bIsOK = LoadData(m_sPath);
        return;
    }
}

wxGISSpatialReferenceValidator::~wxGISSpatialReferenceValidator(void)
{
}

bool wxGISSpatialReferenceValidator::LoadData(wxString sPath)
{
    FILE *fp = VSIFOpenL( sPath.mb_str(wxConvUTF8), "rb");
	if( fp == NULL )
        return false;

    unsigned int nLength;
    VSIFSeekL( fp, 0, SEEK_END );
    nLength = VSIFTellL( fp );
    VSIFSeekL( fp, 0, SEEK_SET );
        
    nLength = MAX(0,nLength);
    char *pszXML = (char *) VSIMalloc(nLength); //+ 1
    if( pszXML == NULL )
    {
        VSIFCloseL(fp);
        wxLogError(_("Failed to allocate %d byte buffer to hold xml file."), nLength );
        return false;
    }

    if( VSIFReadL( pszXML, 1, nLength, fp ) != nLength )
    {
        VSIFCloseL(fp);
        CPLFree( pszXML );
        wxLogError(_("Failed to read %d bytes from xml file."), nLength );
        return false;
    }
        
//    pszXML[nLength] = '\0';
    VSIFCloseL(fp);

    wxMemoryInputStream mis(pszXML, nLength);   

    wxXmlDocument m_pXmlDoc(mis);
    CPLFree( pszXML );
    if(!m_pXmlDoc.IsOk())
        return false;
    wxXmlNode* pRootNode = m_pXmlDoc.GetRoot();
    if(NULL == pRootNode)
        return false;
    wxXmlNode* pChildNode = pRootNode->GetChildren();
    while(pChildNode != NULL)
    {
        wxString sName = pChildNode->GetAttribute(wxT("name"),wxT(""));
        if(!sName.IsEmpty())
        {
            LIMITS lims;
            lims.minx = wxAtof(pChildNode->GetAttribute(wxT("minx"),wxT("-179.99999999")));
            lims.miny = wxAtof(pChildNode->GetAttribute(wxT("miny"),wxT("-89,99999999")));
            lims.maxx = wxAtof(pChildNode->GetAttribute(wxT("maxx"),wxT("179.99999999")));
            lims.maxy = wxAtof(pChildNode->GetAttribute(wxT("maxy"),wxT("89,99999999")));
            m_DataMap[sName] = lims;
       }
        pChildNode = pChildNode->GetNext();
    }
    return true;
}

wxString wxGISSpatialReferenceValidator::GetPath(wxXmlNode* pNode)
{
	if(!pNode)
		return wxEmptyString;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		if( pChildren->GetAttribute(wxT("name"), NONAME) == wxString(wxT("wxGxSpatialReferencesFolder")))
            return pChildren->GetAttribute(wxT("path"), wxT(""));
		pChildren = pChildren->GetNext();
	}
	return wxEmptyString;
}

bool wxGISSpatialReferenceValidator::IsLimitsSet(wxString sProj)
{
    if(m_DataMap.find(sProj) != m_DataMap.end())
        return true;
    return false;
}
*/