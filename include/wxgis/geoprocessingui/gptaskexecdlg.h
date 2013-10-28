/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxTaskExecDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
/*
#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/framework/progressor.h"
#include "wxgis/core/event.h"

#include "wx/wxhtml.h"
#include "wx/imaglist.h"

typedef struct _taskmessage
{
    wxGISEnumMessageType nType;
    wxString sMessage;
}TASKMESSAGE;

typedef std::vector<TASKMESSAGE> TaskMessageArray;

/** \class wxGxTaskExecDlg gptaskexecdlg.h
 *  \brief The dialog showing execution process
 */
/*
class WXDLLIMPEXP_GIS_GPU wxGxTaskExecDlg :
	public wxDialog,
    public ITrackCancel
{
    enum
	{
		ID_CANCEL_PROCESS = wxID_HIGHEST + 30
	};
public:
	wxGxTaskExecDlg(wxGISGPToolManager* pToolManager, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~wxGxTaskExecDlg(void);
    virtual void SetTaskID(int nTaskID);
    virtual void FillHtmlWindow();
    //events
    virtual void OnExpand(wxCommandEvent & event);
    virtual void OnCancel(wxCommandEvent & event);
    virtual void OnCancelTask(wxCommandEvent & event);
	virtual void OnClose(wxCloseEvent & event);
	virtual void OnFinish(wxGISProcessEvent & event);
	//ITrackCancel
	virtual void PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType);
protected:
    wxImageList m_ImageList;
    bool m_bExpand;
    wxGISEnumMessageType m_nState;
    wxHtmlWindow* m_pHtmlWindow;
    wxBoxSizer* m_bMainSizer;
    wxBitmap m_ExpandBitmapBW, m_ExpandBitmap, m_ExpandBitmapBWRotated, m_ExpandBitmapRotated;
    wxBitmapButton* m_bpExpandButton;
    wxStaticText * m_Text;
    wxStaticBitmap* m_pStateBitmap;
    //wxCheckBox* m_pCheckBox;
    wxBitmapButton* m_bpCloseButton;
    TaskMessageArray m_MessageArray;

    wxString m_sHead;
    wxString m_sNote;
    wxIcon m_Icon;

    int m_nTaskID;
    wxGISGPToolManager* m_pToolManager;

    DECLARE_EVENT_TABLE();
};

class WXDLLIMPEXP_GIS_GPU wxGxTaskObject;
/** \class wxGxTaskObjectExecDlg gptaskexecdlg.h
 *  \brief The GxObject showing execution process
 */
/*
class wxGxTaskObjectExecDlg : 
    public wxGxTaskExecDlg
{
public:
	wxGxTaskObjectExecDlg(wxGxTaskObject* pGxTaskObject, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~wxGxTaskObjectExecDlg(void);
    //events
    virtual void OnCancel(wxCommandEvent & event);
    virtual void OnCancelTask(wxCommandEvent & event);
	virtual void OnClose(wxCloseEvent& event);
protected:
    wxGxTaskObject* m_pGxTaskObject;
};

/** \class wxGxTaskObject gptaskexecdlg.h
 *  \brief The GxObject showing execution process
 */
/*
class WXDLLIMPEXP_GIS_GPU wxGxTaskObject :
    public IGxObject,
	public IGxObjectUI,
    public IGxTask,
    public IGxObjectWizard,
    public ITrackCancel,
    public IProgressor,
	public wxEvtHandler
{
public:
	wxGxTaskObject(wxGISGPToolManager* pToolManager, wxString sName, wxIcon LargeToolIcon = wxNullIcon, wxIcon SmallToolIcon = wxNullIcon);
    virtual ~wxGxTaskObject(void);
	//IGxObject
    virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void){return CPLString();};
	virtual wxString GetCategory(void){return wxString(_("Task"));};
	virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog);
	virtual void Detach(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxTaskObject.ContextMenu"));};
	virtual wxString NewMenu(void){return wxEmptyString;};
    //IGxObjectWizard
    virtual bool Invoke(wxWindow* pParentWnd);
    //IProgressor
    virtual void SetValue(int value);
    virtual bool Show(bool bShow){return true;};
    virtual void SetRange(int range){};
    virtual int GetRange(void) const {return 100;};
    virtual int GetValue(void) const {return m_nDonePercent;};
    virtual void Play(void){};
    virtual void Stop(void){};
	virtual void SetYield(bool bYield = false){};
//IGxTask
    virtual wxGISEnumTaskStateType GetState(void);
    virtual wxDateTime GetStart();
    virtual wxDateTime GetFinish();
    virtual double GetDonePercent(){return m_nDonePercent;};
    virtual wxString GetLastMessage();
    virtual bool StartTask();
    virtual bool StopTask();
    virtual bool PauseTask();
    virtual int GetPriority(void);
    virtual void SetPriority(int nNewPriority);
    //wxGxTaskObject
    virtual int GetTaskID(void){return m_nTaskID;};
    virtual void SetTaskID(int nTaskID);
	virtual void ShowProcess(wxWindow* pParentWnd);
	virtual void ShowToolConfig(wxWindow* pParentWnd);
    //ITrackCancel
	virtual void PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType);
	//events
	void OnTaskStateChanged(wxGISProcessEvent & event);
	void OnFinish(wxGISProcessEvent & event);
protected:
    wxGxTaskObjectExecDlg *m_pTaskExecDlg;
    wxString m_sName;
    int m_nDonePercent;
    wxIcon m_LargeToolIcon, m_SmallToolIcon;
    TaskMessageArray m_MessageArray;
    int m_nTaskID;
    wxGISGPToolManager* m_pToolManager;
	long m_nCookie;

    DECLARE_EVENT_TABLE();
};

*/