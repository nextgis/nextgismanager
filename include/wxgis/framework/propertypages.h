/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012,2013 Bishop
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
#pragma once

#include "wx/intl.h"
#include "wx/string.h"
#include "wx/stattext.h"
#include "wx/statline.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/bmpbuttn.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/choice.h"
#include "wx/panel.h"
#include <wx/imaglist.h>
#include <wx/propgrid/propgrid.h>

#include "wxgis/framework/applicationbase.h"
#include "wxgis/core/config.h"


/** \class IPropertyPage framework.h
    \brief A IPropertyPage interface class for property pages in options dialogs.

    This is base class for property page in options dialog.    
*/

class WXDLLIMPEXP_GIS_FRW IPropertyPage : public wxPanel
{
    DECLARE_ABSTRACT_CLASS(IPropertyPage)
public:
    /** \fn virtual ~IPropertyPage(void)
     *  \brief A destructor.
     */
    virtual ~IPropertyPage(void) {};
    /** \fn virtual bool Create(IFrameApplication* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
     *  \brief A Create function.
     *  \param application The main app pointer
     *  \param parent The parent window pointer
     *  \param id The window ID
     *  \param pos The window position
     *  \param size The window size
     *  \param style The window style
     *  \param name The window name
     *  \return true if creation is success, otherwize false
     *  
     *  The PropertyPage is two step creating.
     */
    virtual bool Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel")) = 0;
    /** \fn virtual wxString GetName(void)
     *  \brief Get the property page name.
     *  \return The property page name
     */
    virtual wxString GetPageName(void) = 0;
    /** \fn  virtual void Apply(void)
     *  \brief Executed when OK is pressed
     */
    virtual void Apply(void) = 0;
};

/** \class wxGISMiscPropertyPage framework.h
    \brief The miscellaneous property page.  
*/

class WXDLLIMPEXP_GIS_FRW wxGISMiscPropertyPage : public IPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISMiscPropertyPage)
public:
    wxGISMiscPropertyPage(void);
	~wxGISMiscPropertyPage();
    virtual bool Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Misc_Panel"));
//  IPropertyPage
    virtual wxString GetPageName(void){return wxString(_("Miscellaneous"));};
    virtual void Apply(void);
    //events
    void OnOpenLocPath(wxCommandEvent& event);
    void OnOpenSysPath(wxCommandEvent& event);
    void OnOpenLogPath(wxCommandEvent& event);
protected:
    virtual void FillLangArray(wxString sPath);
protected:
	enum
	{
		ID_LOCPATH = wxID_HIGHEST + 1,
		ID_OPENLOCPATH,
		ID_LANGCHOICE,
		ID_SYSPATH,
		ID_OPENSYSPATH,
		ID_LOGPATH,
		ID_OPENLOGPATH,
        ID_SSTIMEOUT
	};

	wxStaticText* m_staticText1;
	wxTextCtrl* m_LocalePath;
	wxBitmapButton* m_bpOpenLocPath;
	wxStaticText* m_staticText2;
	wxChoice* m_LangChoice;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_SysPath;
	wxBitmapButton* m_bpOpenSysPath;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_LogPath;
	wxBitmapButton* m_bpOpenLogPath;
    wxCheckBox* m_checkDebug;
    wxCheckBox* m_checkShowSS;
	wxStaticBitmap* m_bitmapwarn;
	wxStaticText* m_staticTextWarn;
    wxStaticLine* m_staticline;
	wxTextCtrl* m_Timeout;

    wxGISApplicationBase* m_pApp;
    wxArrayString m_aLangs, m_aLangsDesc;
    wxImageList m_ImageList;

    int m_nTimeout;

    DECLARE_EVENT_TABLE()
};


/** \class wxGISGDALConfPropertyPage framework.h
    \brief The GDAL Configuration Options property page.  
*/

class WXDLLIMPEXP_GIS_FRW wxGISGDALConfPropertyPage : public IPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISGDALConfPropertyPage)
public:
    wxGISGDALConfPropertyPage(void);
	~wxGISGDALConfPropertyPage();
    virtual bool Create(wxGISApplicationBase* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("GDAL_conf_Panel"));
//  IPropertyPage
    virtual wxString GetPageName(void){return wxString(_("GDAL Configuration"));};
    virtual void Apply(void);
protected:
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
protected:
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 1,
	};
protected:
    wxGISApplicationBase* m_pApp;
    wxPropertyGrid* m_pg;
};
