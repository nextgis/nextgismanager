/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/sr_16.xpm"

//--------------------------------------------------------------------------
// wxGISSpatialReferencePropertyPage
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISSpatialReferencePropertyPage, wxGxPropertyPage)

BEGIN_EVENT_TABLE(wxGISSpatialReferencePropertyPage, wxGxPropertyPage)
	EVT_CHILD_FOCUS( wxGISSpatialReferencePropertyPage::OnChildFocus )
END_EVENT_TABLE()

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(void) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("Spatial Reference"));
	m_PageIcon = wxBitmap(sr_16_xpm);
	m_pg = NULL;
}

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxGxPropertyPage()
{
	m_sPageName = wxString(_("Spatial Reference"));
	m_PageIcon = wxBitmap(sr_16_xpm);
	m_pg = NULL;
	
    Create(pTrackCancel, parent, id, pos, size, style, name);
}

wxGISSpatialReferencePropertyPage::~wxGISSpatialReferencePropertyPage()
{
}

void wxGISSpatialReferencePropertyPage::Apply(void)
{
	
}

bool wxGISSpatialReferencePropertyPage::CanApply() const
{
	//TODO: Allow to edit spatial reference
	return false;
}

bool wxGISSpatialReferencePropertyPage::FillProperties(wxGxSelection* const pSel)
{
	if(m_pg)
	{
		m_pg->Clear();
		if(NULL == pSel)
			return false;
			
		wxGxCatalogBase* pCat = GetGxCatalog();	
		wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetFirstSelectedObjectId());	
		if(NULL == pGxObject)
			return false;
			
		wxGISSpatialReference oSRS;	
		//if project file
		wxGxPrjFile* pPrjFile = wxDynamicCast(pGxObject, wxGxPrjFile);
		if(pPrjFile)
		{
			oSRS = pPrjFile->GetSpatialReference();
		}
		
		//if dataset
		if(!oSRS.IsOk())
		{
			wxGxDataset* pGxDataset = wxDynamicCast(pGxObject, wxGxDataset);
			if(pGxDataset)
			{
				wxGISDataset* pDataset = pGxDataset->GetDataset(false, m_pTrackCancel);
				if(pDataset)
					oSRS = pDataset->GetSpatialReference();
				wsDELETE(pDataset);	
			}
		}
			
		//fill propertygrid
		if(oSRS.IsOk())
		{
			oSRS->AutoIdentifyEPSG();
			if (oSRS->IsCompound())
			{
				//search projection srs
				bool bProjAdd = false;
				OGRSpatialReference *poNewSRS;
				const OGR_SRSNode *poProjCS = oSRS->GetAttrNode("PROJCS");
				if (poProjCS != NULL)
				{
					AppendProperty( new wxPropertyCategory(_("Projected Coordinate System")) );
					poNewSRS = new OGRSpatialReference();
					poNewSRS->SetRoot(poProjCS->Clone());
					FillProjected(wxGISSpatialReference(poNewSRS));
					bProjAdd = true;
				}

				bool bGeogAdd = false;
				if (oSRS->GetAttrNode("GEOGCS") != NULL || oSRS->GetAttrNode("GEOCCS") != NULL)
				{
					if (bProjAdd)
					{
						AppendProperty( new wxPropertyCategory(_("Geographic Coordinate System")) );
					}
					wxGISSpatialReference oGeogCS(oSRS->CloneGeogCS());
					FillGeographic(oGeogCS);
					bGeogAdd = true;
				}

				bool bLoclaAdd = false;
				const OGR_SRSNode *poLocalCS = oSRS->GetAttrNode("LOCAL_CS");
				if (poLocalCS != NULL)
				{
					if (bProjAdd || bGeogAdd)
					{
						AppendProperty( new wxPropertyCategory(_("Local Coordinate System")) );
					}

					poNewSRS = new OGRSpatialReference();
					poNewSRS->SetRoot(poLocalCS->Clone());
					FillLoclal(wxGISSpatialReference(poNewSRS));
					bLoclaAdd = true;
				}

				const OGR_SRSNode *poVertCS = oSRS->GetAttrNode("VERT_CS");
				if (poVertCS != NULL)
				{
					if (bProjAdd || bGeogAdd || bLoclaAdd)
					{
						AppendProperty( new wxPropertyCategory(_("Vertical Coordinate System")) );
					}

					poNewSRS = new OGRSpatialReference();
					poNewSRS->SetRoot(poVertCS->Clone());
					FillVertical(wxGISSpatialReference(poNewSRS));
				}
			}
			else if(oSRS->IsProjected())
			{
				AppendProperty(new wxPropertyCategory(_("Projected Coordinate System")));
				FillProjected(oSRS);
				AppendProperty( new wxPropertyCategory(_("Geographic Coordinate System")) );
				wxGISSpatialReference oGeogCS(oSRS->CloneGeogCS());
				FillGeographic(oGeogCS);
			}
			else if (oSRS->IsGeographic() || oSRS->IsGeocentric())
			{
				AppendProperty( new wxPropertyCategory(_("Geographic Coordinate System")) );
				FillGeographic(oSRS);
			}
			//else if(IsGeocentric)
			else if(oSRS->IsLocal())
			{
				AppendProperty(new wxPropertyCategory(_("Local Coordinate System")));
				FillLoclal(oSRS);
			}
			else if (oSRS->IsVertical())
			{
				AppendProperty(new wxPropertyCategory(_("Vertical Coordinate System")));
				FillVertical(oSRS);
			}
			else
				FillUndefined();
			//OSRDestroySpatialReference(oSRS);
		}
		else
		{
			FillUndefined();
		}			
	}
	
	return true;
}

bool wxGISSpatialReferencePropertyPage::Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;
		
	m_pTrackCancel = pTrackCancel;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizerAndFit( bMainSizer );
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
        AppendProperty(new wxStringProperty(wxString(pszAfName, wxConvLocal), wxT("EPSGP"), wxString(pszAfCode, wxConvLocal)));
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
    AppendProperty( new wxStringProperty(_("Name"), wxT("UnitPrjName"), wxString(pszUnitName, wxConvLocal)) );
    AppendProperty(new wxFloatProperty(_("Meters per unit"), wxT("MPUP"), fUnit));

}

void wxGISSpatialReferencePropertyPage::FillVertical(const wxGISSpatialReference &oSRS)
{
    const char *pszName = oSRS->GetAttrValue("VERT_CS");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameVertical"), wxString(pszName, wxConvLocal)) );
    //EPSG
    const char *pszAfCode = oSRS->GetAuthorityCode("VERT_CS");
    const char *pszAfName = oSRS->GetAuthorityName("VERT_CS");
    if (pszAfName || pszAfCode)
        AppendProperty(new wxStringProperty(wxString(pszAfName, wxConvLocal), wxT("EPSGV"), wxString(pszAfCode, wxConvLocal)));


    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Vertical datum")) );
    const char *pszNameDatum = oSRS->GetAttrValue("VERT_DATUM");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameVDatum"), wxString(pszNameDatum, wxConvLocal)) );

    pid = AppendProperty(new wxPropertyCategory(_("Vertical unit")));
    char *pszUnitName = NULL;
    double fUnit = oSRS->GetLinearUnits(&pszUnitName);
    AppendProperty(new wxStringProperty(_("Name"), wxT("UnitVertName"), wxString(pszUnitName, wxConvLocal)));
    AppendProperty(new wxFloatProperty(_("Meters per unit"), wxT("MPUV"), fUnit));

    OGRAxisOrientation eOrientation;
    const char *pszAxisName = oSRS->GetAxis("VERT_CS", 0, &eOrientation);
    if (pszAxisName != NULL)
    {
        pid = AppendProperty(new wxPropertyCategory(_("Axis")));
        AppendProperty(new wxStringProperty(_("Name"), wxT("AxisVName"), wxString(pszAxisName, wxConvLocal)));
        AppendProperty(new wxStringProperty(_("Orientation"), wxT("AxisOrient"), wxString(OSRAxisEnumToName(eOrientation), wxConvLocal)));
    }

}

void wxGISSpatialReferencePropertyPage::FillGeographic(const wxGISSpatialReference &oSRS)
{
    const char *pszName = oSRS->GetAttrValue("GEOGCS");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameGeographic"), wxString(pszName, wxConvLocal)) );
    //EPSG
    const char *pszAfCode = oSRS->GetAuthorityCode("GEOGCS");
    const char *pszAfName = oSRS->GetAuthorityName("GEOGCS");
	if(pszAfName || pszAfCode)
	    AppendProperty( new wxStringProperty(wxString(pszAfName, wxConvLocal), wxT("EPSGG"), wxString(pszAfCode, wxConvLocal))  );

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

    double adfCoeff[7];
    OGRErr eErr = oSRS->GetTOWGS84(adfCoeff);
    if (eErr == OGRERR_NONE)
    {
        wxString sParams;
        for (size_t i = 0; i < 7; ++i)
        {
            sParams.Append(wxString::FromDouble(adfCoeff[i]));
            if (i < 6)
                sParams.Append(wxT(", "));
        }
        pid = AppendProperty( new wxPropertyCategory(_("To WGS parameters")));
        AppendProperty(pid, new wxStringProperty(_("Parameters"), wxT("ToWGSParametersName"), sParams));
    }
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


void wxGISSpatialReferencePropertyPage::OnChildFocus( wxChildFocusEvent& event )
{
	// do nothing to avoid "scrollbar jump" if wx2.9 is used
}
