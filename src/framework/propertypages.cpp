/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012-2014 Dmitry Baryshnikov
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
#include "wxgis/framework/propertypages.h"
#include "wxgis/core/format.h"


#include "wx/valnum.h"
#include <wx/valtext.h>
#include <wx/valgen.h>

#include "../../art/open.xpm"
#include "../../art/state.xpm"

#include <wx/dir.h>

#undef LT_OBJDIR
#include "cpl_multiproc.h"
#include "gdal_priv.h"
#include "gdal.h"

//-------------------------------------------------------------------------------
// IPropertyPage
//-------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(IPropertyPage, wxPanel)

wxString IPropertyPage::GetLastErrorMessage() const
{
    return m_sErrMessage;
}

//-------------------------------------------------------------------------------
// wxGISMiscPropertyPage
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISMiscPropertyPage, IPropertyPage)

BEGIN_EVENT_TABLE(wxGISMiscPropertyPage, wxPanel)
    EVT_BUTTON(ID_OPENLOCPATH, wxGISMiscPropertyPage::OnOpenLocPath)
    EVT_BUTTON(ID_OPENSYSPATH, wxGISMiscPropertyPage::OnOpenSysPath)
    EVT_BUTTON(ID_OPENLOGPATH, wxGISMiscPropertyPage::OnOpenLogPath)
END_EVENT_TABLE()

wxGISMiscPropertyPage::wxGISMiscPropertyPage(void) : m_pApp(NULL)
{
}

wxGISMiscPropertyPage::~wxGISMiscPropertyPage()
{
}

bool wxGISMiscPropertyPage::Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
        return false;

    m_pApp = application;
    if(!m_pApp)
        return false;

    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return false;

    wxBoxSizer* bMainSizer;
    bMainSizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* sbRootSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Splash screen") ), wxVERTICAL );
    m_checkShowSS = new wxCheckBox( this, wxID_ANY, _("Show"), wxDefaultPosition, wxDefaultSize, 0 );
    wxXmlNode* pSplashNode = oConfig.GetConfigNode(enumGISHKCU, wxT("wxGISCommon/splash"));
    bool bShowSplash = GetBoolValue(pSplashNode, wxT("show"), true);

    m_checkShowSS->SetValue(bShowSplash);
    sbRootSizer->Add( m_checkShowSS, 0, wxALL | wxEXPAND, 5 );

    wxBoxSizer* bSizer011 = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText* pStaticText = new wxStaticText( this, wxID_ANY, _("Time out (sec.):"), wxDefaultPosition, wxDefaultSize, 0 );
    pStaticText->Wrap( -1 );
    bSizer011->Add( pStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_nTimeout = GetDecimalValue(pSplashNode, wxT("timeout"), 20000) / 1000;
    m_Timeout = new wxTextCtrl( this, ID_SSTIMEOUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxMakeIntegerValidator(&m_nTimeout) );
    bSizer011->Add( m_Timeout, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    sbRootSizer->Add( bSizer011, 0, wxALL | wxEXPAND, 5 );

    bMainSizer->Add( sbRootSizer, 0, wxALL | wxEXPAND, 5 );


    m_staticText1 = new wxStaticText( this, wxID_ANY, _("Locale files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText1->Wrap( -1 );
    bMainSizer->Add( m_staticText1, 0, wxEXPAND|wxALL, 5 );

    wxBoxSizer* bLocPathSizer;
    bLocPathSizer = new wxBoxSizer( wxHORIZONTAL );

    //locale files path
    m_LocalePath = new wxTextCtrl( this, ID_LOCPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_LocalePath->ChangeValue( oConfig.GetLocaleDir() );
    bLocPathSizer->Add( m_LocalePath, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    m_bpOpenLocPath = new wxBitmapButton( this, ID_OPENLOCPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    bLocPathSizer->Add( m_bpOpenLocPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    bMainSizer->Add( bLocPathSizer, 0, wxEXPAND, 5 );

    m_staticText2 = new wxStaticText( this, wxID_ANY, _("Language"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText2->Wrap( -1 );
    bMainSizer->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );

    //see current locale dir for locales

    FillLangArray(oConfig.GetLocaleDir());
    m_LangChoice = new wxChoice( this, ID_LANGCHOICE, wxDefaultPosition, wxDefaultSize, m_aLangsDesc, 0 );
    int nChoice = m_aLangs.Index(oConfig.GetLocale());
    if(nChoice == wxNOT_FOUND)
    {
        m_LangChoice->SetStringSelection( m_aLangsDesc[0] );
    }
    else
    {
        m_LangChoice->SetStringSelection(m_aLangsDesc[nChoice]);
    }
    bMainSizer->Add( m_LangChoice, 0, wxALL|wxEXPAND, 5 );

    m_staticText3 = new wxStaticText( this, wxID_ANY, _("System files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText3->Wrap( -1 );
    bMainSizer->Add( m_staticText3, 0, wxEXPAND|wxALL, 5 );

    wxBoxSizer* bSizer21;
    bSizer21 = new wxBoxSizer( wxHORIZONTAL );

    m_SysPath = new wxTextCtrl( this, ID_SYSPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_SysPath->ChangeValue( oConfig.GetSysDir() );
    bSizer21->Add( m_SysPath, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_bpOpenSysPath = new wxBitmapButton( this, ID_OPENSYSPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    bSizer21->Add( m_bpOpenSysPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    bMainSizer->Add( bSizer21, 0, wxEXPAND, 5 );

    m_staticText4 = new wxStaticText( this, wxID_ANY, _("Log files folder path"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4->Wrap( -1 );
    bMainSizer->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer211 = new wxBoxSizer( wxHORIZONTAL );
    m_LogPath = new wxTextCtrl( this, ID_LOGPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_LogPath->ChangeValue( oConfig.GetLogDir() );
    bSizer211->Add( m_LogPath, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_bpOpenLogPath = new wxBitmapButton( this, ID_OPENLOGPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    bSizer211->Add( m_bpOpenLogPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    bMainSizer->Add( bSizer211, 0, wxEXPAND, 5 );

    m_checkDebug = new wxCheckBox( this, wxID_ANY, _("Log debug GDAL messages"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkDebug->SetValue(oConfig.GetDebugMode());

    bMainSizer->Add( m_checkDebug, 0, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizerInfo = new wxBoxSizer( wxHORIZONTAL );

    m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    bMainSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );

    m_ImageList.Create(16, 16);
    m_ImageList.Add(wxBitmap(state_xpm));

    m_bitmapwarn = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(3), wxDefaultPosition, wxDefaultSize, 0 );
    bSizerInfo->Add( m_bitmapwarn, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    m_staticTextWarn = new wxStaticText( this, wxID_ANY, _("To apply some changes on this page the application restart needed"), wxDefaultPosition, wxDefaultSize, 0 );
    
    int w = oConfig.ReadInt(enumGISHKCU, GetApplication()->GetAppName() + wxString(wxT("/propertypages/width")), 480);
    
    m_staticTextWarn->Wrap( w - 30 );

    wxFont WarnFont = this->GetFont();
    WarnFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticTextWarn->SetFont(WarnFont);

    bSizerInfo->Add( m_staticTextWarn, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    bMainSizer->Add( bSizerInfo, 0, wxALL|wxEXPAND, 5 );

    this->SetSizerAndFit(bMainSizer);
    this->Layout();

    TransferDataToWindow();

    return true;
}

void wxGISMiscPropertyPage::Apply(void)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        wxGISAppConfig oConfig = GetConfig();
        if(!oConfig.IsOk())
            return;
        wxString sChoice = m_LangChoice->GetStringSelection();
        if(!sChoice.IsEmpty())
        {
            int nIndex = m_aLangsDesc.Index(sChoice);
            if(nIndex != wxNOT_FOUND)
            {
                wxString sLoc = m_aLangs[nIndex];
                wxGISEnumReturnType eType = m_pApp->SetupLoc(m_aLangs[nIndex], m_LocalePath->GetValue());
                
                if(eType == enumGISReturnWarning)
                {
                    m_sErrMessage = wxString::Format(_("Failed to set %s locale, reset to English"), sChoice.c_str());
                    oConfig.SetLocale(wxT("en"));
                    m_pApp->SetupLoc(wxT("en"), m_LocalePath->GetValue());
                }
                else if(eType == enumGISReturnFailed)
                {
                    m_sErrMessage = wxString::Format(_("Failed to set any locale"), sChoice.c_str());
                    oConfig.SetLocale(wxT("en"));
                    m_pApp->SetupLoc(wxT("en"), m_LocalePath->GetValue());
                }
                else
                {
                    oConfig.SetLocale(m_aLangs[nIndex]);
                }
                oConfig.SetLocaleDir(m_LocalePath->GetValue());
            }
        }
        if(m_LogPath->IsModified())
        {
            m_pApp->SetupLog(m_LogPath->GetValue());
            oConfig.SetLogDir(m_LogPath->GetValue());
        }
        if(m_SysPath->IsModified())
        {
            m_pApp->SetupSys(m_SysPath->GetValue());
            oConfig.SetSysDir(m_SysPath->GetValue());
        }
        if(m_checkDebug->GetValue() != oConfig.GetDebugMode())
        {
            m_pApp->SetDebugMode(m_checkDebug->GetValue());
            oConfig.SetDebugMode(m_checkDebug->GetValue());
        }

        wxXmlNode* pSplashNode = oConfig.GetConfigNode(enumGISHKCU, wxT("wxGISCommon/splash"));
        bool bShowSplash = GetBoolValue(pSplashNode, wxT("show"), true);
        if(m_checkShowSS->GetValue() != bShowSplash)
        {
            oConfig.Write(enumGISHKCU, wxT("wxGISCommon/splash/show"), m_checkShowSS->GetValue());
        }

        int nTimeout = GetDecimalValue(pSplashNode, wxT("timeout"), 20000) / 1000;

        if(m_nTimeout != nTimeout)
        {
            oConfig.Write(enumGISHKCU, wxT("wxGISCommon/splash/timeout"), int(m_nTimeout * 1000));
        }
    }
}

void wxGISMiscPropertyPage::FillLangArray(wxString sPath)
{
    m_aLangs.Clear();
    m_aLangsDesc.Clear();

    wxDir dir(sPath);
    if ( !dir.IsOpened() )
        return;

    wxString sSubdirName;
    bool bAddEn(true);
    wxString sEn(wxT("en"));
    bool cont = dir.GetFirst(&sSubdirName, wxEmptyString, wxDIR_DIRS);
    while ( cont )
    {
        if(bAddEn)
            if(sEn.CmpNoCase(sSubdirName) == 0)
                bAddEn = false;
        //
        const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sSubdirName);
        if(loc_info)
        {
            m_aLangs.Add(sSubdirName);
            m_aLangsDesc.Add(loc_info->Description);
        }
        cont = dir.GetNext(&sSubdirName);
    }
    if(bAddEn)
    {
        const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(wxT("en"));
        if(loc_info)
        {
            m_aLangs.Add(wxT("en"));
            m_aLangsDesc.Add(loc_info->Description);
        }
        else
        {
            m_aLangs.Add(wxT("en"));
            m_aLangsDesc.Add(wxT("en"));
        }
    }
}

void wxGISMiscPropertyPage::OnOpenLocPath(wxCommandEvent& event)
{
    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return;
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder with locale files")), oConfig.GetLocaleDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString sPath = dlg.GetPath();
        m_LocalePath->ChangeValue( sPath );
        m_LocalePath->SetModified(true);
        m_LangChoice->Clear();
        FillLangArray(sPath);
        m_LangChoice->Append(m_aLangsDesc);
        m_LangChoice->SetSelection( 0 );
    }
}

void wxGISMiscPropertyPage::OnOpenSysPath(wxCommandEvent& event)
{
    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return;
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS system files folder path")), oConfig.GetSysDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString sPath = dlg.GetPath();
        m_SysPath->ChangeValue( sPath );
        m_SysPath->SetModified(true);
    }
}

void wxGISMiscPropertyPage::OnOpenLogPath(wxCommandEvent& event)
{
    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return;
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a wxGIS log files folder path")), oConfig.GetLogDir(), wxDD_DEFAULT_STYLE |wxDD_DIR_MUST_EXIST );
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString sPath = dlg.GetPath();
        m_LogPath->ChangeValue( sPath );
        m_LogPath->SetModified(true);
    }
}


//-------------------------------------------------------------------------------
// wxGISGDALConfPropertyPage
// http://trac.osgeo.org/gdal/wiki/ConfigOptions
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISGDALConfPropertyPage, IPropertyPage)

wxGISGDALConfPropertyPage::wxGISGDALConfPropertyPage(void) : m_pApp(NULL)
{
}

wxGISGDALConfPropertyPage::~wxGISGDALConfPropertyPage()
{
}

bool wxGISGDALConfPropertyPage::Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
        return false;

    m_pApp = application;
    if(!m_pApp)
        return false;

    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return false;

    wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    m_pg->SetBoolChoices(wxT("ON"), wxT("OFF"));

    //Generic Options
    wxPGProperty* prop = AppendProperty( new wxPropertyCategory(_("Generic Options")) );

    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_DEBUG")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_DEBUG", "OFF")) == 0 ? false : true));
    AppendProperty(prop,  new wxFileProperty(wxString(wxT("CPL_LOG")), wxPG_LABEL, wxString(CPLGetConfigOption( "CPL_LOG", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_LOG_ERRORS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_LOG_ERRORS", "OFF")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_TIMESTAMP")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_TIMESTAMP", "OFF")) == 0 ? false : true));
    AppendProperty(prop,  new wxIntProperty(wxString(wxT("CPL_MAX_ERROR_REPORTS")), wxPG_LABEL, atoi(CPLGetConfigOption( "CPL_MAX_ERROR_REPORTS", "1000" )) ));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_ACCUM_ERROR_MSG")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_ACCUM_ERROR_MSG", "ON")) == 0 ? false : true));
    AppendProperty(prop,  new wxDirProperty(wxString(wxT("CPL_TMPDIR")), wxPG_LABEL, wxString(CPLGetConfigOption( "CPL_TMPDIR", "" ), wxConvUTF8)));
    AppendProperty(prop,  new wxDirProperty(wxString(wxT("GDAL_DATA")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_DATA", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_DISABLE_CPLLOCALEC")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_DISABLE_CPLLOCALEC", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_FILENAME_IS_UTF8")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_FILENAME_IS_UTF8", "ON")) == 0 ? false : true));

    AppendProperty(prop, new wxStringProperty(wxString(wxT("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS")), wxPG_LABEL, wxString(CPLGetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS", ""), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("CPL_ZIP_ENCODING")), wxPG_LABEL, wxString(CPLGetConfigOption("CPL_ZIP_ENCODING", ""), wxConvUTF8)));
#ifdef WIN32
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_API_PROXY_SERVER")), wxPG_LABEL, wxString(CPLGetConfigOption("GDAL_API_PROXY_SERVER", ""), wxConvUTF8)));
#else
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_API_PROXY_SERVER")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_API_PROXY_SERVER", "NO")) == 0 ? false : true));
#endif
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_API_PROXY")), wxPG_LABEL, wxString(CPLGetConfigOption("GDAL_API_PROXY", ""), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_API_PROXY_LINE_CACHING")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_API_PROXY_LINE_CACHING", "YES")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_API_PROXY_FORCE_APPROX")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_API_PROXY_FORCE_APPROX", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("QGIS_HACK")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("QGIS_HACK", "NO")) == 0 ? false : true));

    //web options
    prop = AppendProperty(new wxPropertyCategory(_("Web options")));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("CPL_VSIL_CURL_ALLOWED_EXTENSIONS")), wxPG_LABEL, wxString(CPLGetConfigOption( "CPL_VSIL_CURL_ALLOWED_EXTENSIONS", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_PROXY")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_HTTP_PROXY", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_PROXY_AUTH")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_PROXY_AUTH", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_PROXYUSERPWD")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_HTTP_PROXYUSERPWD", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_USERPWD")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_HTTP_USERPWD", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_UNSAFESSL")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_HTTP_UNSAFESSL", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_USERAGENT")), wxPG_LABEL, wxString(CPLGetConfigOption("GDAL_HTTP_USERAGENT", ""), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GDAL_HTTP_TIMEOUT")), wxPG_LABEL, wxString(CPLGetConfigOption("GDAL_HTTP_TIMEOUT", ""), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_CURL_GZIP")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_CURL_GZIP", "ON")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_CURL_VERBOSE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_CURL_VERBOSE", "OFF")) == 0 ? false : true));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("CPL_VSIL_CURL_MAX_RANGES")), wxPG_LABEL, atoi(CPLGetConfigOption( "CPL_VSIL_CURL_MAX_RANGES", "250" ))));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CPL_VSIL_CURL_SLOW_GET_SIZE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CPL_CURL_VERBOSE", "ON")) == 0 ? false : true));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GOA2_CLIENT_ID")), wxPG_LABEL, wxString(CPLGetConfigOption("GOA2_CLIENT_ID", ""), wxConvUTF8)));
    AppendProperty(prop, new wxStringProperty(wxString(wxT("GOA2_CLIENT_SECRET")), wxPG_LABEL, wxString(CPLGetConfigOption("GOA2_CLIENT_SECRET", ""), wxConvUTF8)));

    //GDAL Options
    prop = AppendProperty(new wxPropertyCategory(_("GDAL Options")) );
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_DISABLE_READDIR_ON_OPEN")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_DISABLE_READDIR_ON_OPEN", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("GDAL_CACHEMAX")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_CACHEMAX", "41943040" ))));

    wxArrayString saParams = wxStringTokenize(wxString(CPLGetConfigOption( "GDAL_SKIP", "" ), wxConvUTF8), wxT(" "), wxTOKEN_RET_EMPTY);
    AppendProperty(prop, new wxArrayStringProperty(wxString(wxT("GDAL_SKIP")), wxPG_LABEL, saParams));

    AppendProperty(prop, new wxDirProperty(wxString(wxT("GDAL_DRIVER_PATH")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_DRIVER_PATH", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_FORCE_CACHING")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_FORCE_CACHING", "OFF")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_VALIDATE_CREATION_OPTIONS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_VALIDATE_CREATION_OPTIONS", "YES")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_IGNORE_AXIS_ORIENTATION")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_IGNORE_AXIS_ORIENTATION", "FALSE")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_USE_SOURCE_OVERVIEWS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_USE_SOURCE_OVERVIEWS", "OFF")) == 0 ? false : true));

    //PAM/AUX
    wxString sCurrentVal(CPLGetConfigOption( "GDAL_PAM_MODE", "PAM" ), wxConvUTF8);
    wxPGChoices chs;
    chs.Add(wxT("PAM"), 1);
    chs.Add(wxT("AUX"), 2);
    AppendProperty(prop, new wxEnumProperty(wxString(wxT("GDAL_PAM_MODE")), wxPG_LABEL, chs, chs.GetValue(chs.Index(sCurrentVal))));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("ESRI_XML_PAM")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("ESRI_XML_PAM", "YES")) == 0 ? false : true));

    AppendProperty(prop, new wxDirProperty(wxString(wxT("GDAL_PAM_PROXY_DIR")), wxPG_LABEL, wxString(CPLGetConfigOption( "GDAL_PAM_PROXY_DIR", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("GDAL_MAX_DATASET_POOL_SIZE")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_MAX_DATASET_POOL_SIZE", "100" ))));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("GDAL_SWATH_SIZE")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_SWATH_SIZE", "10000000" ))));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("GDAL_MAX_BAND_COUNT")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_MAX_BAND_COUNT", "-1" ))));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_USE_SSE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_USE_SSE", "YES")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("USE_RRD")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("USE_RRD", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("GDAL_NUM_THREADS")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_NUM_THREADS", wxString::Format(wxT("%d"), wxThread::GetCPUCount()).c_str() ))));
    AppendProperty(prop, new wxDirProperty(wxString(wxT("TMPDIR")), wxPG_LABEL, wxString(CPLGetConfigOption( "TMPDIR", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxDirProperty(wxString(wxT("TEMP")), wxPG_LABEL, wxString(CPLGetConfigOption( "TEMP", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxIntProperty(wxString(wxT("VSI_CACHE_SIZE")), wxPG_LABEL, atoi(CPLGetConfigOption( "VSI_CACHE_SIZE", "25000000" ))));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("VSI_CACHE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("VSI_CACHE", "FALSE")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_OPENGIS_SCHEMAS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_OPENGIS_SCHEMAS", "OFF")) == 0 ? false : true));

    //TIFF driver
    wxPGProperty* sub_prop = AppendProperty(prop, new wxPropertyCategory(_("GeoTIFF driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_IGNORE_READ_ERRORS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_IGNORE_READ_ERRORS", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("GDAL_TIFF_OVR_BLOCKSIZE")), wxPG_LABEL, atoi(CPLGetConfigOption( "GDAL_TIFF_OVR_BLOCKSIZE", "128" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("CONVERT_YCBCR_TO_RGB")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CONVERT_YCBCR_TO_RGB", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxFloatProperty(wxString(wxT("GTIFF_ALPHA")), wxPG_LABEL, atof(CPLGetConfigOption( "GTIFF_ALPHA", "2" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_DONT_WRITE_BLOCKS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_DONT_WRITE_BLOCKS", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_DIRECT_IO")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_DIRECT_IO", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_TIFF_INTERNAL_MASK")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_TIFF_INTERNAL_MASK", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_TIFF_INTERNAL_MASK_TO_8BIT")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_TIFF_INTERNAL_MASK_TO_8BIT", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("TIFF_USE_OVR")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("TIFF_USE_OVR", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_POINT_GEO_IGNORE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_POINT_GEO_IGNORE", "FALSE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_FORCE_STRIP_CHOP")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_FORCE_STRIP_CHOP", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_REPORT_COMPD_CS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_REPORT_COMPD_CS", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_ENABLE_TIFF_SPLIT")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_ENABLE_TIFF_SPLIT", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxFileProperty(wxString(wxT("GEOTIFF_CSV")), wxPG_LABEL, wxString(CPLGetConfigOption("GEOTIFF_CSV", ""), wxConvUTF8)));

    sCurrentVal = wxString(CPLGetConfigOption( "GDAL_TIFF_ENDIANNESS", "NATIVE" ), wxConvUTF8);
    wxPGChoices chs1;
    chs1.Add(wxT("NATIVE"), 1);
    chs1.Add(wxT("LITTLE"), 2);
    chs1.Add(wxT("BIG"), 3);
    chs1.Add(wxT("INVERTED"), 4);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("GDAL_TIFF_ENDIANNESS")), wxPG_LABEL, chs1, chs1.GetValue(chs1.Index(sCurrentVal))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_DELETE_ON_ERROR")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_DELETE_ON_ERROR", "YES")) == 0 ? false : true));

    //JPEG/LZW/PACKBITS/DEFLATE/CCITTRLE/CCITTFAX3/CCITTFAX4/NONE
    sCurrentVal = wxString(CPLGetConfigOption( "COMPRESS_OVERVIEW", "JPEG" ), wxConvUTF8);
    wxPGChoices chs2;
    chs2.Add(wxT("JPEG"), 1);
    chs2.Add(wxT("LZW"), 2);
    chs2.Add(wxT("PACKBITS"), 3);
    chs2.Add(wxT("DEFLATE"), 4);
    chs2.Add(wxT("CCITTRLE"), 5);
    chs2.Add(wxT("CCITTFAX3"), 6);
    chs2.Add(wxT("CCITTFAX4"), 7);
    chs2.Add(wxT("NONE"), 8);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("COMPRESS_OVERVIEW")), wxPG_LABEL, chs2, chs2.GetValue(chs2.Index(sCurrentVal))));

    //PIXEL|BAND
    sCurrentVal = wxString(CPLGetConfigOption( "INTERLEAVE_OVERVIEW", "PIXEL" ), wxConvUTF8);
    wxPGChoices chs3;
    chs3.Add(wxT("PIXEL"), 1);
    chs3.Add(wxT("BAND"), 2);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("INTERLEAVE_OVERVIEW")), wxPG_LABEL, chs3, chs3.GetValue(chs3.Index(sCurrentVal))));

    //MINISBLACK/MINISWHITE/RGB/CMYK/YCBCR/CIELAB/ICCLAB/ITULAB
    sCurrentVal = wxString(CPLGetConfigOption( "PHOTOMETRIC_OVERVIEW", "YCBCR" ), wxConvUTF8);
    wxPGChoices chs4;
    chs4.Add(wxT("MINISBLACK"), 1);
    chs4.Add(wxT("MINISWHITE"), 2);
    chs4.Add(wxT("RGB"), 2);
    chs4.Add(wxT("CMYK"), 3);
    chs4.Add(wxT("YCBCR"), 4);
    chs4.Add(wxT("CIELAB"), 5);
    chs4.Add(wxT("ICCLAB"), 6);
    chs4.Add(wxT("ITULAB"), 7);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("PHOTOMETRIC_OVERVIEW")), wxPG_LABEL, chs4, chs4.GetValue(chs4.Index(sCurrentVal))));

    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("PREDICTOR_OVERVIEW")), wxPG_LABEL, atoi(CPLGetConfigOption( "PREDICTOR_OVERVIEW", "1" ))));

    //IF_NEEDED|IF_SAFER|YES|NO
    sCurrentVal = wxString(CPLGetConfigOption( "BIGTIFF_OVERVIEW", "IF_NEEDED" ), wxConvUTF8);
    wxPGChoices chs5;
    chs5.Add(wxT("IF_NEEDED"), 1);
    chs5.Add(wxT("IF_SAFER"), 2);
    chs5.Add(wxT("YES"), 3);
    chs5.Add(wxT("NO"), 4);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("BIGTIFF_OVERVIEW")), wxPG_LABEL, chs5, chs5.GetValue(chs5.Index(sCurrentVal))));

    //DEFAULT/BROKEN
    sCurrentVal = wxString(CPLGetConfigOption( "GTIFF_LINEAR_UNITS", "DEFAULT" ), wxConvUTF8);
    wxPGChoices chs6;
    chs6.Add(wxT("DEFAULT"), 1);
    chs6.Add(wxT("BROKEN"), 2);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("GTIFF_LINEAR_UNITS")), wxPG_LABEL, chs6, chs6.GetValue(chs6.Index(sCurrentVal))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GTIFF_ESRI_CITATION")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GTIFF_ESRI_CITATION", "YES")) == 0 ? false : true));

    //HFA driver
    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("HFA driver options")));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("HFA_USE_RRD")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("HFA_USE_RRD", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("USE_SPILL")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("USE_SPILL", "NO")) == 0 ? false : true));

    //JPEG driver
    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("JPEG driver options"))  );
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("JPEG_QUALITY_OVERVIEW")), wxPG_LABEL, atoi(CPLGetConfigOption( "JPEG_QUALITY_OVERVIEW", "75" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("JPEG_WRITE_RGB")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("JPEG_WRITE_RGB", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_JPEG_TO_RGB")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_JPEG_TO_RGB", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("JPEGMEM")), wxPG_LABEL, atoi(CPLGetConfigOption( "JPEGMEM", "524288000" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("DUMP_JP2_BOXES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("DUMP_JP2_BOXES", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_JP2K_ALT_OFFSETVECTOR_ORDER")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_JP2K_ALT_OFFSETVECTOR_ORDER", "FALSE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxFileProperty(wxString(wxT("IDA_COLOR_FILE")), wxPG_LABEL, wxString(CPLGetConfigOption( "IDA_COLOR_FILE", "" ), wxConvUTF8)));
    m_pg->SetPropertyAttribute(wxT("IDA_COLOR_FILE"), wxPG_FILE_WILDCARD, "Color table (*.clr)|*.clr" );

    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GDAL_ONE_BIG_READ")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_ONE_BIG_READ", "NO")) == 0 ? false : true));

    //GRIB driver
    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("GRIB driver options"))  );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GRIB_NORMALIZE_UNITS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GRIB_NORMALIZE_UNITS", "OFF")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("Other GDAL options"))  );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("VRT_ALLOW_MEM_DRIVER")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("VRT_ALLOW_MEM_DRIVER", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OZI_APPROX_GEOTRANSFORM")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OZI_APPROX_GEOTRANSFORM", "NO")) == 0 ? false : true));

    //OGR Options
    prop = AppendProperty( new wxPropertyCategory(_("OGR Options")) );
    double dfStep = atof(CPLGetConfigOption("OGR_ARC_STEPSIZE","4")) / 180 * PI;
    AppendProperty(prop, new wxFloatProperty(wxString(wxT("OGR_ARC_STEPSIZE")), wxPG_LABEL, dfStep));
    AppendProperty(prop, new wxFloatProperty(wxString(wxT("OGR_ARC_MINLENGTH")), wxPG_LABEL, atof( CPLGetConfigOption("OGR_ARC_MINLENGTH","0") )));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GDAL_FIX_ESRI_WKT")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GDAL_FIX_ESRI_WKT", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OSR_USE_ETMERC")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OSR_USE_ETMERC", "FALSE")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OSR_USE_CT_GRAMMAR")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OSR_USE_CT_GRAMMAR", "TRUE")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OVERRIDE_PROJ_DATUM_WITH_TOWGS84")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OVERRIDE_PROJ_DATUM_WITH_TOWGS84", "YES")) == 0 ? false : true));


#if (defined(WIN32) || defined(WIN32CE)) && !defined(__MINGW32__)
#  define PROJLIBNAME      "proj.dll"
#elif defined(__MINGW32__)
// XXX: If PROJ.4 library was properly built using libtool in Cygwin or MinGW
// environments it has the interface version number embedded in the file name
// (it is CURRENT-AGE number). If DLL came somewhere else (e.g. from MSVC
// build) it can be named either way, so use PROJSO environment variable to
// specify the right library name. By default assume that in Cygwin/MinGW all
// components were buit in the same way.
#  define PROJLIBNAME      "libproj-0.dll"
#elif defined(__CYGWIN__)
#  define PROJLIBNAME      "cygproj-0.dll"
#elif defined(__APPLE__)
#  define PROJLIBNAME      "libproj.dylib"
#else
#  define PROJLIBNAME      "libproj.so"
#endif
    AppendProperty(prop, new wxStringProperty(wxString(wxT("PROJSO")), wxPG_LABEL, wxString(CPLGetConfigOption( "PROJSO", PROJLIBNAME ), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("USE_PROJ_480_FEATURES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("USE_PROJ_480_FEATURES", "YES")) == 0 ? false : true));
    AppendProperty(prop, new wxFloatProperty(wxString(wxT("CENTER_LONG")), wxPG_LABEL, atof( CPLGetConfigOption("CENTER_LONG","0.0") )));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("CHECK_WITH_INVERT_PROJ")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("CHECK_WITH_INVERT_PROJ", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxFloatProperty(wxString(wxT("THRESHOLD")), wxPG_LABEL, atof( CPLGetConfigOption("THRESHOLD","1000.0") )));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OGR_SETFIELD_NUMERIC_WARNING")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_SETFIELD_NUMERIC_WARNING", "NO")) == 0 ? false : true));

    //SKIP/ONLY_CCW/DEFAULT
    sCurrentVal = wxString(CPLGetConfigOption( "OGR_ORGANIZE_POLYGONS", "DEFAULT" ), wxConvUTF8);
    wxPGChoices chs7;
    chs7.Add(wxT("SKIP"), 1);
    chs7.Add(wxT("ONLY_CCW"), 2);
    chs7.Add(wxT("DEFAULT"), 3);
    AppendProperty(prop, new wxEnumProperty(wxString(wxT("OGR_ORGANIZE_POLYGONS")), wxPG_LABEL, chs7, chs7.GetValue(chs7.Index(sCurrentVal))));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OGR_ENABLE_PARTIAL_REPROJECTION")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_ENABLE_PARTIAL_REPROJECTION", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("OGR_FORCE_ASCII")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_FORCE_ASCII", "YES")) == 0 ? false : true));
    saParams = wxStringTokenize(wxString(CPLGetConfigOption( "OGR_SKIP", "" ), wxConvUTF8), wxT(" "), wxTOKEN_RET_EMPTY);
    AppendProperty(prop, new wxArrayStringProperty(wxString(wxT("OGR_SKIP")), wxPG_LABEL, saParams));
    AppendProperty(prop, new wxDirProperty(wxString(wxT("OGR_DRIVER_PATH")), wxPG_LABEL, wxString(CPLGetConfigOption( "OGR_DRIVER_PATH", "" ), wxConvUTF8)));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GEOMETRY_AS_COLLECTION")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GEOMETRY_AS_COLLECTION", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("ATTRIBUTES_SKIP")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("ATTRIBUTES_SKIP", "NO")) == 0 ? false : true));
    AppendProperty(prop, new wxBoolProperty(wxString(wxT("GEOJSON_FLATTEN_GEOCOUCH")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GEOJSON_FLATTEN_GEOCOUCH", "TRUE")) == 0 ? false : true));

    //OGR Options
    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("DXF driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("DXF_INLINE_BLOCKS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("DXF_INLINE_BLOCKS", "TRUE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("DXF_HEADER_ONLY")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("DXF_HEADER_ONLY", "FALSE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("DXF_ENCODING")), wxPG_LABEL, wxString(CPLGetConfigOption( "DXF_ENCODING", "" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("DXF_WRITE_HATCH")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("DXF_WRITE_HATCH", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("DXF_MERGE_BLOCK_GEOMETRIES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("DXF_MERGE_BLOCK_GEOMETRIES", "TRUE")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("GML driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GMLJP2OVERRIDE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GMLJP2OVERRIDE", "OFF")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_FACE_HOLE_NEGATIVE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_FACE_HOLE_NEGATIVE", "NO")) == 0 ? false : true));
    //AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("GMLJP2OVERRIDE")), wxPG_LABEL, wxString(CPLGetConfigOption( "GMLJP2OVERRIDE", "r" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_GET_SECONDARY_GEOM")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_GET_SECONDARY_GEOM", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("GML_FIELDTYPES")), wxPG_LABEL, wxString(CPLGetConfigOption( "GML_FIELDTYPES", "" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_EXPOSE_GML_ID")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_EXPOSE_GML_ID", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_EXPOSE_FID")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_EXPOSE_FID", "NO")) == 0 ? false : true));
    //STANDARD/SEQUENTIAL_LAYERS/INTERLEAVED_LAYERS
    sCurrentVal = wxString(CPLGetConfigOption( "GML_READ_MODE", "STANDARD" ), wxConvUTF8);
    wxPGChoices chs8;
    chs8.Add(wxT("STANDARD"), 1);
    chs8.Add(wxT("SEQUENTIAL_LAYERS"), 2);
    chs8.Add(wxT("INTERLEAVED_LAYERS"), 3);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("GML_READ_MODE")), wxPG_LABEL, chs8, chs8.GetValue(chs8.Index(sCurrentVal))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_INVERT_AXIS_ORDER_IF_LAT_LONG")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_INVERT_AXIS_ORDER_IF_LAT_LONG", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_CONSIDER_EPSG_AS_URN")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_CONSIDER_EPSG_AS_URN", "NO")) == 0 ? false : true));

    //EXPAT/XERCES
    sCurrentVal = wxString(CPLGetConfigOption( "GML_PARSER", "EXPAT" ), wxConvUTF8);
    wxPGChoices chs9;
    chs9.Add(wxT("EXPAT"), 1);
    chs9.Add(wxT("XERCES"), 2);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("GML_PARSER")), wxPG_LABEL, chs9, chs9.GetValue(chs9.Index(sCurrentVal))));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("GML_SAVE_RESOLVED_TO")), wxPG_LABEL, wxString(CPLGetConfigOption( "GML_SAVE_RESOLVED_TO", NULL ), wxConvUTF8)));

    //EXPAT/XERCES
    sCurrentVal = wxString(CPLGetConfigOption( "GML_SKIP_RESOLVE_ELEMS", "ALL" ), wxConvUTF8);
    wxPGChoices chs10;
    chs10.Add(wxT("ALL"), 1);
    chs10.Add(wxT("HUGE"), 2);
    chs10.Add(wxT("NONE"), 3);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("GML_SKIP_RESOLVE_ELEMS")), wxPG_LABEL, chs10, chs10.GetValue(chs10.Index(sCurrentVal))));

    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("GML_GFS_TEMPLATE")), wxPG_LABEL, wxString(CPLGetConfigOption( "GML_GFS_TEMPLATE", NULL ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_HUGE_TEMPFILE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_HUGE_TEMPFILE", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_DOWNLOAD_WFS_SCHEMA")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_DOWNLOAD_WFS_SCHEMA", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_USE_OLD_FID_FORMAT")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_USE_OLD_FID_FORMAT", "FALSE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GML_FETCH_ALL_GEOMETRIES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GML_FETCH_ALL_GEOMETRIES", "NO")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("GPX driver options")) );
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("GPX_N_MAX_LINKS")), wxPG_LABEL, atoi(CPLGetConfigOption( "GPX_N_MAX_LINKS", "2" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GPX_ELE_AS_25D")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GPX_ELE_AS_25D", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GPX_SHORT_NAMES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GPX_SHORT_NAMES", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("GPX_USE_EXTENSIONS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("GPX_USE_EXTENSIONS", "FALSE")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("KML driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("KML_DEBUG")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("KML_DEBUG", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_USE_DOC.KML")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_USE_DOC.KML", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_READ_GROUND_OVERLAY")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_READ_GROUND_OVERLAY", "yes")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_RESOLVE_STYLE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_RESOLVE_STYLE", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_EXTERNAL_STYLE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_EXTERNAL_STYLE", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_NAME_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_NAME_FIELD", "Name" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_DESCRIPTION_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_DESCRIPTION_FIELD", "description" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_TIMESTAMP_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_TIMESTAMP_FIELD", "timestamp" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_BEGIN_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_BEGIN_FIELD", "begin" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_END_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_END_FIELD", "end" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_ALTITUDEMODE_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_ALTITUDEMODE_FIELD", "altitudeMode" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_TESSELLATE_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_TESSELLATE_FIELD", "tessellate" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_EXTRUDE_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_EXTRUDE_FIELD", "extrude" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_VISIBILITY_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_VISIBILITY_FIELD", "visibility" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_DRAWORDER_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_DRAWORDER_FIELD", "drawOrder" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("LIBKML_ICON_FIELD")), wxPG_LABEL, wxString(CPLGetConfigOption( "LIBKML_ICON_FIELD", "icon" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_LAUNDER_FIELD_NAMES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_LAUNDER_FIELD_NAMES", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("LIBKML_WRAPDATELINE")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("LIBKML_WRAPDATELINE", "NO")) == 0 ? false : true));


    //normal/highlight
    sCurrentVal = wxString(CPLGetConfigOption( "LIBKML_STYLEMAP_KEY", "normal" ), wxConvUTF8);
    wxPGChoices chs11;
    chs11.Add(wxT("normal"), 1);
    chs11.Add(wxT("highlight"), 2);
    AppendProperty(sub_prop, new wxEnumProperty(wxString(wxT("LIBKML_STYLEMAP_KEY")), wxPG_LABEL, chs11, chs11.GetValue(chs11.Index(sCurrentVal))));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("PG driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_LIST_ALL_TABLES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_LIST_ALL_TABLES", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("PGCLIENTENCODING")), wxPG_LABEL, wxString(CPLGetConfigOption( "PGCLIENTENCODING", NULL ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_USE_POSTGIS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_USE_POSTGIS", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_SKIP_VIEWS")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_SKIP_VIEWS", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_USE_GEOGRAPHY")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_USE_GEOGRAPHY", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_PG_RETRIEVE_FID")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_PG_RETRIEVE_FID", "TRUE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("PGSQL_OGR_FID")), wxPG_LABEL, wxString(CPLGetConfigOption( "PGSQL_OGR_FID", "ogc_fid" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_USE_BASE64")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_USE_BASE64", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_USE_COPY")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_USE_COPY", "NO")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("PG_USE_TEXT")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("PG_USE_TEXT", "NO")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("WFS driver options")) );
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_WFS_PAGING_ALLOWED")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_WFS_PAGING_ALLOWED", "OFF")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("OGR_WFS_PAGE_SIZE")), wxPG_LABEL, atoi(CPLGetConfigOption( "OGR_WFS_PAGE_SIZE", "100" ))));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("OGR_WFS_BASE_START_INDEX")), wxPG_LABEL, atoi(CPLGetConfigOption( "OGR_WFS_BASE_START_INDEX", "0" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_WFS_LOAD_MULTIPLE_LAYER_DEFN")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_WFS_LOAD_MULTIPLE_LAYER_DEFN", "TRUE")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_WFS_FIX_MAXFEATURES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_WFS_FIX_MAXFEATURES", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_WFS_USE_STREAMING")), wxPG_LABEL, CSLTestBoolean( CPLGetConfigOption( "OGR_WFS_USE_STREAMING", "YES" ) ) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("OSM driver options")));
    AppendProperty(sub_prop, new wxFileProperty(wxString(wxT("OSM_CONFIG_FILE")), wxPG_LABEL, wxString(CPLGetConfigOption("OSM_CONFIG_FILE", ""), wxConvUTF8)));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("OSM_MAX_TMPFILE_SIZE")), wxPG_LABEL, atoi(CPLGetConfigOption("OSM_MAX_TMPFILE_SIZE", "100"))));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("SQLite options")));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("OGR_SQLITE_CACHE")), wxPG_LABEL, atoi(CPLGetConfigOption( "OGR_SQLITE_CACHE", "0" ))));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("SQLITE_LIST_ALL_TABLES")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("SQLITE_LIST_ALL_TABLES", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("OGR_SQLITE_LIST_VIRTUAL_OGR")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("OGR_SQLITE_LIST_VIRTUAL_OGR", "YES")) == 0 ? false : true));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("SXF options")));
    AppendProperty(sub_prop, new wxBoolProperty(wxString(wxT("SXF_LAYER_FULLNAME")), wxPG_LABEL, CSLTestBoolean(CPLGetConfigOption("SQLITE_LIST_ALL_TABLES", "YES")) == 0 ? false : true));
    AppendProperty(sub_prop, new wxFileProperty(wxString(wxT("RSC_FILENAME")), wxPG_LABEL, wxString(CPLGetConfigOption("RSC_FILENAME", ""), wxConvUTF8)));

    sub_prop = AppendProperty(prop, new wxPropertyCategory(_("Other OGR options")) );
    AppendProperty(sub_prop, new wxStringProperty(wxString(wxT("SHAPE_ENCODING")), wxPG_LABEL, wxString(CPLGetConfigOption( "SHAPE_ENCODING", "" ), wxConvUTF8)));
    AppendProperty(sub_prop, new wxIntProperty(wxString(wxT("OGR_VRT_MAX_OPENED")), wxPG_LABEL, atoi(CPLGetConfigOption( "OGR_VRT_MAX_OPENED", "100" ))));

    m_pg->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

    this->SetSizerAndFit(bMainSizer);
    this->Layout();

    return true;
}

void wxGISGDALConfPropertyPage::Apply(void)
{
}

wxPGProperty* wxGISGDALConfPropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    //pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISGDALConfPropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    //pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

//-------------------------------------------------------------------------------
// wxGISNetworkPropertyPage
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISNetworkPropertyPage, IPropertyPage)

BEGIN_EVENT_TABLE(wxGISNetworkPropertyPage, wxPanel)
    EVT_BUTTON(ID_OPENCACHEPATH, wxGISNetworkPropertyPage::OnOpenCachePath)
END_EVENT_TABLE()

wxGISNetworkPropertyPage::wxGISNetworkPropertyPage(void)
{
    m_pApp = NULL;
}

wxGISNetworkPropertyPage::~wxGISNetworkPropertyPage()
{
}

bool wxGISNetworkPropertyPage::Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
        return false;

    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
        return false;

    m_pApp = application;

    //fill values
    wxString sProxy = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/curl/proxy"), wxEmptyString);
    if (oConfig.ReadBool(enumGISHKCU, wxT("wxGISCommon/curl/proxy/use"), false) && sProxy.Find(':') != wxNOT_FOUND)
    {
        wxArrayString saProxy = wxStringTokenize(sProxy, wxT(":"), wxTOKEN_RET_EMPTY);
        m_sProxyAddress = saProxy[0];
        m_nProxyPort = wxAtoi(saProxy[1]);
    }
    else
    {
        m_sProxyAddress = wxEmptyString;
        m_nProxyPort = 0;
    }

    m_bUseProxy = oConfig.ReadBool(enumGISHKCU, wxT("wxGISCommon/curl/proxy/use"), false);
    
    m_bSSLVerify = oConfig.ReadBool(enumGISHKCU, wxT("wxGISCommon/curl/ssl_verify"), true);
    m_nTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/timeout"), 1000);
    m_nConnectTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/connect_timeout"), 30);
    m_nDNSCacheTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/dns_cache_timeout"), 180);
    m_nCacheDepth = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/cache_depth"), 2);
    m_nWMSTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/timeout"), 300);
    m_sHTTPCodes = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/zero_block_http_codes"), wxString(wxT("204,404")));
    m_sWMSCachePath = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/cache_path"), oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(wxT("cache")));

    wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_staticText1 = new wxStaticText(this, wxID_ANY, _("Timeout (sec.)"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText1->Wrap(-1);
    fgSizer1->Add(m_staticText1, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_timeout = new wxTextCtrl(this, ID_M_TIMEOUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldTimeout = wxMakeIntegerValidator(&m_nTimeout);
    vldTimeout.SetMin(1);
    vldTimeout.SetMax(10000);
    m_timeout->SetValidator(vldTimeout);

    fgSizer1->Add(m_timeout, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText2 = new wxStaticText(this, wxID_ANY, _("Connect timeout (sec.)"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(-1);
    fgSizer1->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_ConnectTimeout = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldConnectTimeout = wxMakeIntegerValidator(&m_nConnectTimeout);
    vldConnectTimeout.SetMin(1);
    vldConnectTimeout.SetMax(10000);
    m_ConnectTimeout->SetValidator(vldConnectTimeout);

    fgSizer1->Add(m_ConnectTimeout, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText3 = new wxStaticText(this, wxID_ANY, _("DNS cache timeout (sec.)"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText3->Wrap(-1);
    fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_dnsCacheTimeout = new wxTextCtrl(this, ID_M_DNSCACHETIMEOUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldDNSCacheTimeout = wxMakeIntegerValidator(&m_nDNSCacheTimeout);
    vldDNSCacheTimeout.SetMin(1);
    vldDNSCacheTimeout.SetMax(10000);
    m_dnsCacheTimeout->SetValidator(vldDNSCacheTimeout);

    fgSizer1->Add(m_dnsCacheTimeout, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    bMainSizer->Add(fgSizer1, 0, wxEXPAND, 5);

    m_sslVerifyCheck = new wxCheckBox(this, ID_M_SSSVERIFYCHECK, _("SSL verify"), wxDefaultPosition, wxDefaultSize, 0);
    m_sslVerifyCheck->SetValidator(wxGenericValidator(&m_bSSLVerify));

    bMainSizer->Add(m_sslVerifyCheck, 0, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* sbSizer1;
    sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Proxy")), wxVERTICAL);

    wxCheckBox *pUseProxyCheck = new wxCheckBox(this, ID_M_USEPROXYCHECK, _("Use proxy"), wxDefaultPosition, wxDefaultSize, 0);
    pUseProxyCheck->SetValidator(wxGenericValidator(&m_bUseProxy));
    sbSizer1->Add(pUseProxyCheck, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* bProxySizer = new wxBoxSizer(wxHORIZONTAL);

    m_staticText4 = new wxStaticText(this, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText4->Wrap(-1);
    bProxySizer->Add(m_staticText4, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxString ipAddressFilter[11] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "." }; // authorized characters for IP Address
    wxArrayString arraystrIPAddress(11, ipAddressFilter);

    wxTextValidator txtvldIPAddress(wxFILTER_INCLUDE_CHAR_LIST, &m_sProxyAddress); //wxFILTER_ALPHANUMERIC text validator for IP Address
    txtvldIPAddress.SetIncludes(arraystrIPAddress); // sets authorized characters for IP Address

    m_ProxyAddress = new wxTextCtrl(this, ID_M_PROXYADDRESS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_ProxyAddress->SetValidator(txtvldIPAddress);

    bProxySizer->Add(m_ProxyAddress, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText5 = new wxStaticText(this, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText5->Wrap(-1);
    bProxySizer->Add(m_staticText5, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_port = new wxTextCtrl(this, ID_M_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldPort = wxMakeIntegerValidator(&m_nProxyPort);
    vldPort.SetMin(0);
    vldPort.SetMax(65535);
    m_port->SetValidator(vldPort);

    bProxySizer->Add(m_port, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    sbSizer1->Add(bProxySizer, 0, wxEXPAND, 5);

    bMainSizer->Add(sbSizer1, 0, wxEXPAND, 5);

    wxStaticBoxSizer* sbSizer2;
    sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("WMS")), wxVERTICAL);

    wxFlexGridSizer* fgSizer2;
    fgSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    fgSizer2->SetFlexibleDirection(wxBOTH);
    fgSizer2->AddGrowableCol(1);
    fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_staticText6 = new wxStaticText(this, wxID_ANY, _("Cache path"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText6->Wrap(-1);
    fgSizer2->Add(m_staticText6, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    //m_textCtrl6 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    //fgSizer2->Add(m_textCtrl6, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* bCachePathSizer = new wxBoxSizer(wxHORIZONTAL);
    m_cachePath = new wxTextCtrl(this, ID_CACHEPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &m_sWMSCachePath));    
    bCachePathSizer->Add(m_cachePath, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_bpOpenCachePath = new wxBitmapButton(this, ID_OPENCACHEPATH, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bCachePathSizer->Add(m_bpOpenCachePath, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    fgSizer2->Add(bCachePathSizer, 0, wxEXPAND, 5);

    m_staticText7 = new wxStaticText(this, wxID_ANY, _("Cache depth"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText7->Wrap(-1);
    fgSizer2->Add(m_staticText7, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_cacheDepth = new wxTextCtrl(this, ID_M_CAHEDEPTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldCacheDepth = wxMakeIntegerValidator(&m_nCacheDepth);
    vldCacheDepth.SetMin(1);
    vldCacheDepth.SetMax(5);
    m_cacheDepth->SetValidator(vldCacheDepth);
    fgSizer2->Add(m_cacheDepth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText8 = new wxStaticText(this, wxID_ANY, _("Timeout (sec.)"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText8->Wrap(-1);
    fgSizer2->Add(m_staticText8, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_WMSTimeout = new wxTextCtrl(this, ID_M_WMSTIMEOUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxIntegerValidator<int> vldWMSTimeout = wxMakeIntegerValidator(&m_nWMSTimeout);
    vldWMSTimeout.SetMin(1);
    vldWMSTimeout.SetMax(10000);
    m_WMSTimeout->SetValidator(vldWMSTimeout);
    fgSizer2->Add(m_WMSTimeout, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText9 = new wxStaticText(this, wxID_ANY, _("Zero Block HTTP Codes"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText9->Wrap(-1);
    fgSizer2->Add(m_staticText9, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    m_httpCodes = new wxTextCtrl(this, ID_M_HTTPCODES, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    wxString codesFilter[11] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "," }; // authorized characters
    wxArrayString arraystrCodes(11, codesFilter);

    wxTextValidator txtvldCodes(wxFILTER_INCLUDE_CHAR_LIST, &m_sHTTPCodes);
    txtvldCodes.SetIncludes(arraystrCodes);

    m_httpCodes->SetValidator(txtvldCodes);

    fgSizer2->Add(m_httpCodes, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    sbSizer2->Add(fgSizer2, 1, wxEXPAND, 5);

    bMainSizer->Add(sbSizer2, 1, wxEXPAND, 5);

    this->SetSizerAndFit(bMainSizer);
    this->Layout();

    TransferDataToWindow();
    return true;
}

void wxGISNetworkPropertyPage::Apply(void)
{
    if (m_port->GetValue().IsEmpty())
        m_port->SetValue(wxT("0"));
    if ( Validate() && TransferDataFromWindow() )
    {
        wxGISAppConfig oConfig = GetConfig();
        if(!oConfig.IsOk())
            return;

        if (!m_sProxyAddress.IsEmpty())
            oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/proxy"), wxString::Format(wxT("%s:%d"), m_sProxyAddress.c_str(), m_nProxyPort));
        else
            oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/proxy"), wxString(wxT("")));

        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/proxy/use"), m_bUseProxy);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/ssl_verify"), m_bSSLVerify);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/timeout"), m_nTimeout);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/connect_timeout"), m_nConnectTimeout);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/curl/dns_cache_timeout"), m_nDNSCacheTimeout);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/cache_depth"), m_nCacheDepth);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/timeout"), m_nWMSTimeout);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/zero_block_http_codes"), m_sHTTPCodes);
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/GDAL/WMS/cache_path"), m_sWMSCachePath);
    }
}

void wxGISNetworkPropertyPage::OnOpenCachePath(wxCommandEvent& event)
{
    wxDirDialog dlg(dynamic_cast<wxWindow*>(m_pApp), wxString(_("Choose a folder for cache")), m_sWMSCachePath, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString sPath = dlg.GetPath();
        m_cachePath->ChangeValue(sPath);
        m_cachePath->SetModified(true);
    }
}