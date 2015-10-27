/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Create Remote Database dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014-2015 Dmitry Baryshnikov
*   Copyright (C) 2014-2015 NextGIS
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

#include "wxgis/catalogui/remoteconndlgs.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"

#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/imaglist.h>
#include <wx/tglbtn.h>

#ifdef wxGIS_USE_POSTGRES

/** @class wxGISCreateDBDlg

    The dialog to create remote database connection, test it and store the connection information in file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISCreateDBDlg : public wxGISRemoteDBConnDlg
{
public:
    wxGISCreateDBDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Create new Remote Database"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~wxGISCreateDBDlg();
protected:	//events
    virtual void OnTest(wxCommandEvent& event);
	virtual void OnOK(wxCommandEvent& event);
protected:
    void CreateUI(bool bHasConnectionPath = true);
protected:
    wxStaticText *m_DBNameStaticText;
    wxStaticText *m_DBConnectionNameStaticText;
    wxStaticText *m_DBTemplateNameStaticText;
    wxTextCtrl *m_DBName;
    wxChoice *m_TempateChoice;

    wxString m_sDBName;
    wxString m_sDBTempate;
};

#endif //wxGIS_USE_POSTGRES

/** @class wxGISBaseImportPanel
 *  
 *  The base import dataset panel.
 * 
 * 	@library{catalogui}
*/
class WXDLLIMPEXP_GIS_CLU wxGISBaseImportPanel : 
	public wxPanel,
	public ITrackCancel
{
    DECLARE_ABSTRACT_CLASS(wxGISBaseImportPanel)
public:
	wxGISBaseImportPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL );
    virtual ~wxGISBaseImportPanel();
    virtual void PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType);
	virtual wxString GetLastMessage(void) const;
	virtual wxGISEnumMessageType GetLastMessageType() const;
	virtual wxGISDataset* GetDataset() const = 0;
	virtual wxString GetDatasetName() const;
	//events
    virtual void OnClose(wxCommandEvent& event);
protected:
    wxStaticBitmap* m_pStateBitmap;
	wxBitmapButton* m_pCloseBitmap;
	wxBoxSizer *m_bMainSizer;
	wxImageList m_ImageList;
	wxGISEnumMessageType m_eCurrentType;
	wxString m_sCurrentMsg;
	wxString m_sDatasetName;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGISDatasetImportDlg

    The dialog to configurate importing datasets (raster or vector) - new names, set encodings, set bands, etc. 

    @library{catalogui}
*/
class  WXDLLIMPEXP_GIS_CLU wxGISDatasetImportDlg : public wxDialog
{
public:
	typedef struct _bands{
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;
	} BANDS;
	typedef struct _datasetdescr{
		wxGISDataset* pDataset;
		wxString sName;
		OGRwkbGeometryType eFilterGeometryType;
		bool bToMultigeomOrAutoCrop;
        bool bSkipInvalidGeometry;
		BANDS nRGBABands;
	} DATASETDESCR;	
	wxGISDatasetImportDlg(wxGxObjectContainer *pDestDs, wxVector<IGxDataset*> &paDatasets, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Configure import datasets"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISDatasetImportDlg();
	virtual size_t GetDatasetCount();
	virtual DATASETDESCR GetDataset(size_t nIndex) const;
protected:
	wxBoxSizer *m_bMainSizer;
	wxVector<DATASETDESCR> m_paDatasets;
};

/** @class wxGISVectorImportPanel
 *  
 *  The import vector dataset panel.
 * 
 * 	@library{catalogui}
*/
class WXDLLIMPEXP_GIS_CLU wxGISVectorImportPanel : public wxGISBaseImportPanel
{
	enum
    {
        ID_ENCODING = wxID_HIGHEST + 4001,
        ID_CHECKVALIDGEOM,
		ID_TEST   
    };
    DECLARE_CLASS(wxGISVectorImportPanel)
public:
	wxGISVectorImportPanel( wxGISFeatureDataset *pSrcDs, wxGxObjectContainer *pDestDs, const wxString &sOutName, OGRwkbGeometryType eFilterGeomType, bool bToMulti, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL );
    virtual ~wxGISVectorImportPanel();
	virtual wxGISDataset* GetDataset() const;
	virtual OGRwkbGeometryType GetFilterGeometryType() const;
	virtual bool GetToMulti() const;
    virtual bool GetSkipInvalidGeometry() const;
	//events
	virtual void OnEncodingSelect(wxCommandEvent& event);
	virtual void OnTestEncoding(wxCommandEvent& event);
protected:
	wxGISFeatureDataset *m_pFeatureClass;
	std::map<wxString, wxFontEncoding> m_mnEnc;
	wxChoice* m_pEncodingsCombo;
    wxCheckBox* m_pSkipInvalidGeometry;
	OGRwkbGeometryType m_eFilterGeometryType;
    bool m_bToMulti, m_bSkipGeomValid;
	wxButton *m_pTestButton;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGISRasterImportPanel
 *  
 *  The import raster dataset panel.
 * 
 * 	@library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISRasterImportPanel : public wxGISBaseImportPanel
{
	enum
    {
        ID_CROP = wxID_HIGHEST + 4001
    };
    DECLARE_CLASS(wxGISRasterImportPanel)
public:
	wxGISRasterImportPanel( wxGISRasterDataset *pSrcDs, wxGxObjectContainer *pDestDs, const wxString &sOutName, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL );
    virtual ~wxGISRasterImportPanel();
	virtual wxGISDataset* GetDataset() const;	
	virtual bool GetAutoCrop() const;	
	virtual wxGISDatasetImportDlg::BANDS GetBands() const;
	//events
	virtual void OnCrop(wxCommandEvent& event);
protected:
	wxGISRasterDataset *m_pRasterDataset;	
	wxChoice* m_pRedBandCombo;
	wxChoice* m_pGreenBandCombo;
	wxChoice* m_pBlueBandCombo;
	wxChoice* m_pAlphaBandCombo;
	wxToggleButton *m_pCropButton;
private:
    DECLARE_EVENT_TABLE()
};


/** @class wxGISTableImportPanel
 *  
 *  The import table panel.
 * 
 * 	@library{catalogui}
 */
class WXDLLIMPEXP_GIS_CLU wxGISTableImportPanel : public wxGISBaseImportPanel
{
	enum
    {
        ID_ENCODING = wxID_HIGHEST + 4001,
		ID_TEST   
    };
    DECLARE_CLASS(wxGISTableImportPanel)
public:
	wxGISTableImportPanel( wxGISTable *pSrcDs, wxGxObjectContainer *pDestDs, const wxString &sOutName, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL );
    virtual ~wxGISTableImportPanel();
	virtual wxGISDataset* GetDataset() const;
	//events
	virtual void OnEncodingSelect(wxCommandEvent& event);
	virtual void OnTestEncoding(wxCommandEvent& event);
protected:
	wxGISTable *m_pTable;
	std::map<wxString, wxFontEncoding> m_mnEnc;
	wxChoice* m_pEncodingsCombo;
	wxButton *m_pTestButton;
private:
    DECLARE_EVENT_TABLE()
};


/** @class wxGISDatasetTestEncodingDlg

    The dialog to test encoding in Vector dataset 

    @library{catalogui}
*/
class  wxGISDatasetTestEncodingDlg : public wxDialog
{
public:
	wxGISDatasetTestEncodingDlg(wxGISTable  *const pDSet, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISDatasetTestEncodingDlg();
};


