/******************************************************************************
 * Project:  wxGIS (Task Manager)
 * Purpose:  Task manager class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2014 Dmitry Baryshnikov
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

#include "wxgis/base.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"
#include "wxgis/tskmngr_app/net.h"

#include <wx/ffile.h>
#include <wx/hashmap.h>

#define TSKDIR wxT("tasks")

class wxGISTaskCategory;
WX_DECLARE_STRING_HASH_MAP(wxGISTaskCategory*, wxGISTaskCategoryMap);

/** @class wxGISTaskManager

    The main task manager class.
*/

class wxGISTaskManager :
    public wxObject,
    public INetEventProcessor
{
    DECLARE_CLASS(wxGISTaskManager)
    friend class wxGISTaskCategory;
public:
	wxGISTaskManager( void );
	virtual ~wxGISTaskManager(void);
    virtual void SetExitState(wxGISNetCommandState nExitState);
    //void LoadTasks(const wxString & sPath);
    virtual bool Init(void);
    virtual void Exit(void);
    virtual void SendNetMessage(const wxNetMessage &msg, int nUserId = wxNOT_FOUND);
    virtual int GetExecTaskCount(void) const;
    virtual int GetMaxExecTaskCount(void) const;
    virtual void SetMaxExecTaskCount(int nMaxExecTasks);
    virtual void OnCategoryExecutionFinished(const wxGISTaskCategory* pCat);
    //INetEventProcessor
    virtual void ProcessNetEvent(wxGISNetEvent& event);
protected:
    virtual void LoadCategories(const wxString &sPathToCategories);
    virtual void DestroyCategories();
    virtual void ProcessNetCommand(const wxNetMessage &msg, int nUserId);
    virtual void OnExit(void);
    virtual wxJSONValue GetChildrenAsJSON(const wxJSONValue &val);
    virtual wxJSONValue GetParamsAsJSON(const wxJSONValue &val);
    virtual wxString GetNewStorePath(const wxString &sAddToName, const wxString &sSubDir = wxEmptyString);
    static wxString ReplaceForbiddenCharsInFileName(const wxString &name, const wxString &ch = wxT("_") );
protected:
    wxGISLocalNetworkService* m_pNetworkService;
    wxGISNetCommandState m_nExitState;
    wxString m_sUserConfigDir;
    wxGISTaskCategoryMap m_omCategories;
    int m_nMaxExecTasks;
    bool m_bExitState;
};
