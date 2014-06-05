/******************************************************************************
 * Project:  wxGIS
 * Purpose:  About Dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Dmitry Baryshnikov
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

#include "wxgis/app/gisaboutdlg.h"
#include "wxgis/core/config.h"
#include "wxgisdefs.h"

#include "../../../art/logo.xpm"

#include <wx/version.h>
#include <wx/ffile.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#ifdef wxGIS_USE_PROJ
    #include "proj_api.h"
#endif

#ifdef wxGIS_USE_GEOS
    #include "geos_c.h"
#endif

#ifdef wxGIS_USE_CAIRO
    #include "cairo.h"
#endif

#ifdef wxGIS_USE_POSTGRES
    #include "libpq-fe.h"
#endif
///////////////////////////////////////////////////////////////////////////////
/// Class wxGISSimpleTextPanel
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISSimpleTextPanel, wxPanel)
   EVT_TEXT_URL(ID_EDIT,wxGISSimpleTextPanel::edtUrlClickUrl)
END_EVENT_TABLE()

wxGISSimpleTextPanel::wxGISSimpleTextPanel( wxString soText, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer = new wxBoxSizer( wxVERTICAL );

	m_pStaticText = new wxTextCtrl( this, ID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_AUTO_URL, wxDefaultValidator, wxT("GISSimpleText") );
	m_pStaticText->AppendText(soText);
	bSizer->Add( m_pStaticText, 1, wxALL|wxEXPAND, 10 );

	this->SetSizer( bSizer );
	this->Layout();

	m_pStaticText->SetSelection(0,0);
}

wxGISSimpleTextPanel::~wxGISSimpleTextPanel()
{
}

void wxGISSimpleTextPanel::edtUrlClickUrl(wxTextUrlEvent& event)
{
    if ( event.GetMouseEvent().LeftUp() ) // we open the browser only when the Left mouse button is pressed and then released
    {
        wxString url = m_pStaticText->GetValue();
        url = url.Mid( event.GetURLStart(), event.GetURLEnd() - event.GetURLStart() ); // we are extracting the clicked url
        ::wxLaunchDefaultBrowser( url );
    }

    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISAboutDialog
///////////////////////////////////////////////////////////////////////////////

wxGISAboutDialog::wxGISAboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    IApplication* pApp = dynamic_cast<IApplication*>(parent);
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxColour BackColor( 255, 255, 255 );

	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* bHeadSizer = new wxBoxSizer( wxHORIZONTAL );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxBitmap(logo_xpm), wxPoint( -1,-1 ), wxDefaultSize, wxNO_BORDER );
	m_bitmap->SetForegroundColour( BackColor );
	m_bitmap->SetBackgroundColour( BackColor );

	bHeadSizer->Add( m_bitmap, 0, 0, 5 );

	//wxGenericStaticText* pTitle = new wxGenericStaticText(this, wxID_ANY, wxString::Format(_("\nwxGIS [%s] (x64)\nVersion: %s"), pApp->GetAppName(), pApp->GetAppVersionString()), wxDefaultPosition, wxDefaultSize, 0);

#ifdef _WIN64
    m_title = new wxGenericStaticText( this, wxID_ANY, wxString::Format(_("\n%s (x64)\nVersion: %s"), pApp->GetAppDisplayName(), pApp->GetAppVersionString()), wxDefaultPosition, wxDefaultSize, 0 );
#else
     m_title = new wxGenericStaticText( this, wxID_ANY, wxString::Format(_("\n%s (x86)\nVersion: %s"), pApp->GetAppDisplayName(), pApp->GetAppVersionString()), wxDefaultPosition, wxDefaultSize, 0 );
#endif

    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_title->SetFont(titleFont);
	m_title->Wrap( -1 );
	m_title->SetBackgroundColour( BackColor );

	bHeadSizer->Add( m_title, 1, wxEXPAND, 5 );

	bMainSizer->Add( bHeadSizer, 0, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bMainSizer->Add( m_staticline1, 0, wxEXPAND|wxALL, 5 );

#ifdef wxGIS_USE_GEOS
    wxString sGEOSStr = wxString(GEOSversion(), wxConvLocal);
#endif

#ifdef wxGIS_USE_PROJ
    wxString sPrjStr = wxString(pj_get_release(), wxConvLocal);
#endif

#ifdef wxGIS_USE_POSTGRES
    wxString sPQStr = wxString::Format(_("Version of the libpq library in use %d"), PQlibVersion());
#endif

    wxString sGDALStr = wxString(GDALVersionInfo("--version"), wxConvLocal);
    wxString sWXStr = wxGetLibraryVersionInfo().GetVersionString();


#ifdef wxGIS_USE_CAIRO
    wxString sCAIROStr = wxString(cairo_version_string(), wxConvLocal);
#endif

	m_AuiNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS );

#ifdef _WIN64
    wxString sAboutApp = wxString::Format(_("%s (x64)\n\nVersion: %s\n\nBuild: %s\n\n(c) 2009-%d Dmitry Baryshnikov (Bishop)\n\nhttp://wxgis.googlecode.com/"), pApp->GetAppDisplayName().c_str(), pApp->GetAppVersionString().c_str(), wxString(__DATE__,wxConvLibc).c_str(),  __YEAR__);
#else
    wxString sAboutApp = wxString::Format(_("%s (x86)\n\nVersion: %s\n\nBuild: %s\n\n(c) 2009-%d Dmitry Baryshnikov (Bishop)\n\nhttp://wxgis.googlecode.com/"), pApp->GetAppDisplayName().c_str(), pApp->GetAppVersionString().c_str(), wxString(__DATE__,wxConvLibc).c_str(),  __YEAR__);
#endif

	m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sAboutApp, m_AuiNotebook), _("About application"));

    wxLongLong nFreeMem = wxGetFreeMemory();
    wxString sFreeMem = wxFileName::GetHumanReadableSize(wxULongLong(nFreeMem.GetHi(), nFreeMem.GetLo()));
    //long dFreeMem =  wxMemorySize(wxGetFreeMemory() / 1048576).ToLong();
	wxString sGdalDrivers;
	for(size_t i = 0; i < GDALGetDriverCount(); ++i)
	{
		wxString sDrvName(GDALGetDriverLongName(GDALGetDriver(i)), wxConvUTF8);
		sGdalDrivers += wxT("    - ");
		sGdalDrivers += sDrvName;
		sGdalDrivers += wxT("\n");
	}
	wxString sOgrDrivers;
	for(size_t i = 0; i < OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount(); ++i)
	{
		OGRSFDriver *pDrv = OGRSFDriverRegistrar::GetRegistrar()->GetDriver(i);
		wxString sDrvName(pDrv->GetName(), wxConvUTF8);
		sOgrDrivers += wxT("    - ");
		sOgrDrivers += sDrvName;
		sOgrDrivers += wxT("\n");
	}
	wxString sAboutSys = wxString::Format(_("HOST '%s'\n\nOS desc - %s\n\nFree memory - %s\n\nLibs:\n\%s"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), sFreeMem.c_str(), sGDALStr.c_str() );
#ifdef wxGIS_USE_GEOS
    sAboutSys += wxT("\nGEOS ");
    sAboutSys += sGEOSStr;
#endif

#ifdef wxGIS_USE_PROJ
    sAboutSys += wxT("\nPROJ ");
    sAboutSys += sPrjStr;
#endif

#ifdef wxGIS_USE_POSTGRES
    sAboutSys += wxT("\nPostgreSQL: ");
    sAboutSys += sPQStr;
#endif

#ifdef wxGIS_USE_CAIRO
    sAboutSys += wxT("\nCAIRO ");
    sAboutSys += sCAIROStr;
#endif

    sAboutSys += wxT("\n");
    sAboutSys += sWXStr;
	sAboutSys += wxString(_("\n\nGDAL Drivers:\n"));
	sAboutSys += sGdalDrivers;
	sAboutSys += wxString(_("\nOGR Drivers:\n"));
	sAboutSys += sOgrDrivers;
	sAboutSys += wxString(_("\n* - The drivers only compiled in GDAL lib and may not supported by wxGIS."));
    m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sAboutSys, m_AuiNotebook), _("About system"));

    //add translation page
    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        wxString sTranslatorsFile = oConfig.GetLocaleDir() + wxFileName::GetPathSeparator() + wxString(wxT("TRANSLATORS.txt"));
        wxFFile file(sTranslatorsFile, wxString(wxT("r")));
	    if(file.IsOpened())
	    {
            wxString sTranslators;
            if(file.ReadAll(&sTranslators, wxConvUTF8))
            {
                m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sTranslators, m_AuiNotebook), _("Translators"));
            }
        }
        wxString sThanksFile = oConfig.GetLocaleDir() + wxFileName::GetPathSeparator() + wxString(wxT("THANKS.txt"));
        wxFFile fanksfile(sThanksFile, wxString(wxT("r")));
	    if(fanksfile.IsOpened())
	    {
            wxString sThanks;
            if(fanksfile.ReadAll(&sThanks, wxConvUTF8))
            {
 	            m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sThanks, m_AuiNotebook), _("Thanks"));
            }
        }
    }

	m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(_("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\nSee the GNU General Public License for more details.\nYou should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>"), m_AuiNotebook), _("License"));

	bMainSizer->Add( m_AuiNotebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, wxString(_("Close")) );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizer->Realize();
	bMainSizer->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();
	this->Centre( wxBOTH );
}

wxGISAboutDialog::~wxGISAboutDialog()
{
}
