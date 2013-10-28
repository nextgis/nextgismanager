/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/catalogui/spatrefpropertypage.h"

//--------------------------------------------------------------------------
// wxGISSpatialReferencePropertyPage
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISSpatialReferencePropertyPage, wxPanel)

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(void)
{
}

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(const wxGISSpatialReference &oSRS, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Create(oSRS, parent, id, pos, size, style, name);
}

wxGISSpatialReferencePropertyPage::~wxGISSpatialReferencePropertyPage()
{
}

bool wxGISSpatialReferencePropertyPage::Create(const wxGISSpatialReference &oSRS, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;

    //if(!oSRS)
    //    return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    //fill propertygrid
    AppendProperty( new wxPropertyCategory(_("Coordinate System")) );
    if(oSRS.IsOk())
    {
        oSRS->AutoIdentifyEPSG();
        if(oSRS->IsProjected())
        {
            FillProjected(oSRS);
            AppendProperty( new wxPropertyCategory(_("Geographic Coordinate System")) );
            wxGISSpatialReference oGeogCS(oSRS->CloneGeogCS());
            FillGeographic(oGeogCS);
        }
        else if(oSRS->IsGeographic())
        {
            FillGeographic(oSRS);
        }
        else if(oSRS->IsLocal())
        {
            FillLoclal(oSRS);
        }
        //else if(pSpaRef->IsVertical())
        //{
        //}
        else
            FillUndefined();
        //OSRDestroySpatialReference(oSRS);
    }
    else
    {
        FillUndefined();
    }

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

void wxGISSpatialReferencePropertyPage::FillUndefined(void)
{
    m_pg->Append( new wxStringProperty(_("Name"), wxT("Name_Undefined"), _("Undefined")) );
}

void wxGISSpatialReferencePropertyPage::FillProjected(const wxGISSpatialReference &oSRS)
{
    const char *pszName = oSRS->GetAttrValue("PROJCS");
    //make bold!  wxPG_BOLD_MODIFIED
    AppendProperty( new wxStringProperty(_("Name"), wxT("Name_Projected"), wxString(pszName, wxConvLocal)) );

    //EPSG
    const char *pszAfCode = oSRS->GetAuthorityCode("PROJCS");
    const char *pszAfName = oSRS->GetAuthorityName("PROJCS");
	if(pszAfName || pszAfCode)
		AppendProperty( new wxStringProperty(wxString(pszAfName, wxConvLocal), wxPG_LABEL, wxString(pszAfCode, wxConvLocal))  );
	//

    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Projection")) );
    const char *pszNameProj = oSRS->GetAttrValue("PROJECTION");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameProj"), wxString(pszNameProj, wxConvLocal)) );
    wxPGProperty* pidparm = AppendProperty(pid, new wxPropertyCategory(_("Parameters")) );

    //ogr_srs_api.h
    AppendProjParam(pidparm, SRS_PP_CENTRAL_MERIDIAN, oSRS);
    AppendProjParam(pidparm, SRS_PP_SCALE_FACTOR, oSRS);
    AppendProjParam(pidparm, SRS_PP_STANDARD_PARALLEL_1, oSRS);
    AppendProjParam(pidparm, SRS_PP_STANDARD_PARALLEL_2, oSRS);
    AppendProjParam(pidparm, SRS_PP_PSEUDO_STD_PARALLEL_1, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_CENTER, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_CENTER, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_ORIGIN, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_ORIGIN, oSRS);
    AppendProjParam(pidparm, SRS_PP_FALSE_EASTING, oSRS);
    AppendProjParam(pidparm, SRS_PP_FALSE_NORTHING, oSRS);
    AppendProjParam(pidparm, SRS_PP_AZIMUTH, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_1, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_1, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_2, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_2, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_3, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_3, oSRS);
    AppendProjParam(pidparm, SRS_PP_RECTIFIED_GRID_ANGLE, oSRS);
    AppendProjParam(pidparm, SRS_PP_LANDSAT_NUMBER, oSRS);
    AppendProjParam(pidparm, SRS_PP_PATH_NUMBER, oSRS);
    AppendProjParam(pidparm, SRS_PP_PERSPECTIVE_POINT_HEIGHT, oSRS);
    AppendProjParam(pidparm, SRS_PP_SATELLITE_HEIGHT, oSRS);
    AppendProjParam(pidparm, SRS_PP_FIPSZONE, oSRS);
    AppendProjParam(pidparm, SRS_PP_ZONE, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_1ST_POINT, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_1ST_POINT, oSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_2ND_POINT, oSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_2ND_POINT, oSRS);

    AppendProperty( new wxPropertyCategory(_("Linear unit")) );
    char *pszUnitName = NULL;
    double fUnit = oSRS->GetLinearUnits(&pszUnitName);
    AppendProperty( new wxStringProperty(_("Name"), wxT("UnitName"), wxString(pszUnitName, wxConvLocal)) );
    AppendProperty( new wxFloatProperty(_("Meters per unit"), wxPG_LABEL, fUnit) );

}

void wxGISSpatialReferencePropertyPage::FillGeographic(const wxGISSpatialReference &oSRS)
{
    const char *pszName = oSRS->GetAttrValue("GEOGCS");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameGeographic"), wxString(pszName, wxConvLocal)) );
    //EPSG
    const char *pszAfCode = oSRS->GetAuthorityCode("GEOGCS");
    const char *pszAfName = oSRS->GetAuthorityName("GEOGCS");
	if(pszAfName || pszAfCode)
	    AppendProperty( new wxStringProperty(wxString(pszAfName, wxConvLocal), wxPG_LABEL, wxString(pszAfCode, wxConvLocal))  );

    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Datum")) );
    const char *pszNameDatum = oSRS->GetAttrValue("DATUM");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameDatum"), wxString(pszNameDatum, wxConvLocal)) );
    wxPGProperty* pidsph = AppendProperty(pid, new wxPropertyCategory(_("Spheroid")) );
    const char *pszNameSph = oSRS->GetAttrValue("SPHEROID");
    AppendProperty(pidsph, new wxStringProperty(_("Name"), wxT("NameSph"), wxString(pszNameSph, wxConvLocal)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Semimajor Axis"), wxPG_LABEL, oSRS->GetSemiMajor(NULL)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Semiminor Axis"), wxPG_LABEL, oSRS->GetSemiMinor(NULL)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Inverse Flattering"), wxPG_LABEL, oSRS->GetInvFlattening(NULL)) );
    pid = AppendProperty( new wxPropertyCategory(_("Angular unit")) );

    char *pszUnitName = NULL;
    double fUnit = oSRS->GetAngularUnits(&pszUnitName);
    AppendProperty( new wxStringProperty(_("Name"), wxT("UnitName"), wxString(pszUnitName, wxConvLocal)) );
    AppendProperty( new wxFloatProperty(_("Radians per unit"), wxPG_LABEL, fUnit) );

    pid = AppendProperty( new wxPropertyCategory(_("Prime Meridian")));

    char *pszMerName = NULL;
    double fMerLon = oSRS->GetPrimeMeridian(&pszMerName);
    AppendProperty(pid, new wxStringProperty(_("Name"), wxT("MerName"), wxString(pszMerName, wxConvLocal)));
    AppendProperty(pid, new wxFloatProperty(_("Longitude"), wxPG_LABEL, fMerLon));
}

void wxGISSpatialReferencePropertyPage::FillLoclal(const wxGISSpatialReference &oSRS)
{
    AppendProperty(new wxStringProperty(_("Name"), wxT("NameLoclal"), _("Undefined")));
}

void wxGISSpatialReferencePropertyPage::AppendProjParam(wxPGProperty* pid, const char *pszName, const wxGISSpatialReference &oSRS)
{
    if(oSRS->FindProjParm(pszName) != wxNOT_FOUND)
    {
        double fParam = oSRS->GetNormProjParm(pszName);
        AppendProperty(pid, new wxFloatProperty(wxString(pszName, wxConvLocal), wxPG_LABEL, fParam));
    }
}

wxPGProperty* wxGISSpatialReferencePropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISSpatialReferencePropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}
