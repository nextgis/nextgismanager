/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPropertyDlg class - dialog/dock window with the GxObject properties.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#pragma once

#include "wxgis/catalogui/catalogui.h"

/*
#include "wxgis/framework/applicationbase.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/gxeventui.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/aui/auibook.h>

/** @class wxGxPropertyPage
 * 
 *  The abstract class of all GxObject Property Pages
 * 
 * @library{catalogui}
 */
 /*
class wxGxPropertyPage : public wxPanel
{
    DECLARE_ABSTRACT_CLASS(wxGxPropertyPage)
public:
	wxGxPropertyPage();
    /** \fn virtual ~wxGxPropertyPage(void)
     *  \brief A destructor.
     *//*
    virtual ~wxGxPropertyPage(void);	
	virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel")) = 0;
    /** \fn virtual wxString GetPageName(void)
     *  \brief Get the property page name.
     *  \return The property page name
     *//*
    virtual wxString GetPageName(void) const;
    /** \fn  virtual void Apply(void)
     *  \brief Executed when Apply is pressed
     *//*
	virtual wxBitmap GetIcon() const; 
    virtual void Apply(void) = 0;
	virtual bool CanApply() const = 0;
	virtual bool CanMerge() const;
	virtual bool FillProperties(wxGxSelection* const pSel) = 0;
protected:
	ITrackCancel* m_pTrackCancel;
	bool m_bCanMerge;
	wxString m_sPageName;
	wxBitmap m_PageIcon;
};

/** @class wxGISPropertyDlg

    The wxGISPropertyDlg class is dialog/panel window with the GxObject properties

    @library{catalogui}
 *//*

class wxGISPropertyDlg : 
	public wxPanel,
	public ITrackCancel
{
protected:
	enum
	{
		ID_WXGPROPERTYDLG = 1001
	};
	DECLARE_DYNAMIC_CLASS(wxGISPropertyDlg)
public:
	wxGISPropertyDlg(void);
	wxGISPropertyDlg( wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISPropertyDlg();
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("PropertyView"));
	virtual void Update(wxGxSelection* const pSel);
	//events
	virtual void OnApply(wxCommandEvent& event);
	virtual void OnApplyUI(wxUpdateUIEvent& event);
protected:
	wxBoxSizer* m_bMainSizer;
	wxAuiNotebook* m_pTabs;
	wxButton* m_sdbSizerApply;
	wxXmlNode* m_pConf;
protected:
    wxString m_sAppName;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxAxPropertyView

    The wxAxPropertyView dock window with GxObject properties

    @library{catalogui}
*//*

class WXDLLIMPEXP_GIS_CLU wxAxPropertyView :
	public wxGISPropertyDlg,
	public IView
{
protected:
    DECLARE_DYNAMIC_CLASS(wxAxPropertyView)
public:
    wxAxPropertyView(void);
	wxAxPropertyView(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxAxPropertyView(void);
//IView
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE | wxTAB_TRAVERSAL, const wxString& name = wxT("PropertyView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual void Refresh(void){};
	virtual wxString GetViewName(void) const {return m_sViewName;};
	virtual wxIcon GetViewIcon(void){return wxNullIcon;};
	virtual void SetViewIcon(wxIcon Icon){};
	//ITrackCancel	
	virtual IProgressor* const GetProgressor(void);
	virtual void PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType);
	//events
	void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
	wxString m_sViewName;
    wxGISApplicationBase* m_pApp;
	wxGxSelection* m_pSelection;
	long m_ConnectionPointSelectionCookie;
private:
    DECLARE_EVENT_TABLE()
};
*/
