/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISTask and wxGISTaskCategories classes
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/net/netfactory.h"
#include "wxgis/net/netevent.h"
#include "wxgis/geoprocessing/tskmngr.h"
#include "wxgis/core/json/jsonval.h"

#include <wx/dynarray.h>

/** \class wxGISTaskMessage task.h
    \brief The Task message.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskMessage
{
public:
    wxGISTaskMessage(long nId = wxNOT_FOUND, const wxString &sMsg = wxEmptyString, wxGISEnumMessageType nType = enumGISMessageUnk, const wxDateTime &dt = wxDateTime::Now())
    {
        m_nId = nId;
        m_sMessage = sMsg;
        m_dt = dt;
        m_nType = nType;
    }

    wxGISTaskMessage(const wxGISTaskMessage& Msg)
    {
        m_sMessage = Msg.m_sMessage;
        m_dt = Msg.m_dt;
        m_nType = Msg.m_nType;
        m_nId = Msg.m_nId;
    }

    void operator = ( const wxGISTaskMessage& Msg )
    {
        m_sMessage = Msg.m_sMessage;
        m_dt = Msg.m_dt;
        m_nType = Msg.m_nType;
        m_nId = Msg.m_nId;
    }

    wxString m_sMessage;
    wxDateTime m_dt;
    wxGISEnumMessageType m_nType;
    long m_nId;
};

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxGISTaskMessage, wxGISTaskMessagesArray, WXDLLIMPEXP_GIS_GP);

class WXDLLIMPEXP_GIS_GP wxGISTaskEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_ADD, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_DEL, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_CHNG, wxGISTaskEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_GP, wxGISTASK_MESSAGEADDED, wxGISTaskEvent);

/** \class wxGISTaskEvent task.h
    \brief The Task event.
*/
class WXDLLIMPEXP_GIS_GP wxGISTaskEvent : public wxEvent
{
public:
    wxGISTaskEvent(int nTaskId = 0, wxEventType eventType = wxGISTASK_ADD, long nMessageId = wxNOT_FOUND) : wxEvent(nTaskId, eventType)
	{        
        m_nMessageId = nMessageId;
	}
	wxGISTaskEvent(const wxGISTaskEvent& event) : wxEvent(event), m_nMessageId(event.m_nMessageId)
	{        
	}
    virtual wxEvent *Clone() const { return new wxGISTaskEvent(*this); }
    long GetMessageId(void) const {return m_nMessageId;};
protected:
    long m_nMessageId;
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxGISTaskEvent)
};

typedef void (wxEvtHandler::*wxGISTaskEventFunction)(wxGISTaskEvent&);

#define wxGISTaskEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxGISTaskEventFunction, func)

#define EVT_GISTASK_ADD(func)  wx__DECLARE_EVT0(wxGISTASK_ADD, wxGISTaskEventHandler(func))
#define EVT_GISTASK_DEL(func)  wx__DECLARE_EVT0(wxGISTASK_DEL, wxGISTaskEventHandler(func))
#define EVT_GISTASK_CHNG(func)  wx__DECLARE_EVT0(wxGISTASK_CHNG, wxGISTaskEventHandler(func))
#define EVT_GISTASK_MESSAGEADDED(func)  wx__DECLARE_EVT0(wxGISTASK_MESSAGEADDED, wxGISTaskEventHandler(func))

class wxGISTaskBase;
WX_DECLARE_HASH_MAP( int, wxGISTaskBase*, wxIntegerHash, wxIntegerEqual, wxGISTaskMap );

/** \class wxGISTaskBase task.h
    \brief The base task class.
*/

class WXDLLIMPEXP_GIS_GP wxGISTaskBase : 
    public wxObject,
    public wxGISConnectionPointContainer
{
    DECLARE_CLASS(wxGISTaskBase)
public:
    wxGISTaskBase(wxGISTaskBase *pParentTask = NULL);
    virtual ~wxGISTaskBase(void);
    virtual wxString GetName() const;
    virtual wxString GetLastError() const;
    virtual int GetId(void) const;
    virtual wxGISEnumTaskStateType GetState(void) const;
    virtual bool Delete(void);
    /**\fn
     * \brief Get full Task config, include subtasks as JSON text. 
     */
    virtual wxJSONValue GetConfig(void);
    virtual void NetMessage(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    virtual size_t GetSubTaskCount(void) const;
    virtual wxGISTaskBase* GetSubTask(size_t nIndex) const;
    virtual wxGISTaskBase* GetSubTask(int nId) const;
    virtual int GetRunTaskCount(void) const;
    virtual void ChangeTask(const wxJSONValue &val);
    virtual void AddTask(const wxJSONValue &val);
    //
    virtual void OnSubTaskChanged(int nId);
    //NET messages
    virtual void SendNetMessageAsync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    virtual wxGISNetCommandState SendNetMessageSync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
protected:
    virtual void Delete(wxGISTaskBase *pTask);
    virtual void ClearTasks(void);
    virtual void NetCommand(wxGISNetCommandState eCmdState, const wxJSONValue &val);
    virtual void NetNote(wxGISNetCommandState eCmdState, const wxJSONValue &val);
protected:
    wxGISTaskBase *m_pParentTask;
    int m_nId;
    wxString m_sName;
    wxGISEnumTaskStateType m_nState;
    wxString m_sLastError;
    wxGISTaskMap m_omSubTasks;
};


/** @class wxGISTask

    The task is class which controls executing geoprocessing operation (tool). The tasks add to the quere. Then user start executing the task, the task state changes to the quered and task is waiting for the task manager tool. The task really starts it state changes to the working.

    @library{wxgisgp}
*/

class WXDLLIMPEXP_GIS_GP wxGISTask : 
    public wxGISTaskBase
{
    DECLARE_CLASS(wxGISTask)
public:
    wxGISTask(wxGISTaskBase *pParentTask = NULL, const wxJSONValue &TaskConfig = wxJSONValue());
    virtual ~wxGISTask(void);
    virtual wxString GetDescription(void) const;
    virtual wxString GetExecutable(void) const;
    virtual wxDateTime GetDateBegin(void) const;
    virtual wxDateTime GetDateEnd(void) const;
    virtual wxDateTime GetDateCreated(void) const;
    virtual wxULongLong GetVolume(void) const;
    virtual long GetPriority(void) const;
    virtual void SetPriority(long nPriority);
    virtual double GetDone(void) const;
    virtual void SetState(wxGISEnumTaskStateType eState);
    /**\fn
     * \brief Get the Task parameters. Any additional values, to standard Name, Description, DateBegin, DateEnd, etc. 
     */
    virtual wxJSONValue GetParameters(void) const;
    /**\fn
     * \brief Get full Task config, include subtasks. 
     */
    virtual wxJSONValue GetConfig(void);
    virtual void AddSubTask(wxGISTask* pTask);

    virtual void ChangeTask(const wxJSONValue &val);
    virtual wxGISTaskMessagesArray GetMessages(void) const;
    virtual wxGISTaskMessage GetMessage(long nMessageId) const;
    virtual void AddMessage(wxGISTaskMessage* pMessage);
    virtual bool StartTask();
    virtual bool StopTask();
protected:
    virtual void NetNote(wxGISNetCommandState eCmdState, const wxJSONValue &val);
protected:
    wxString m_sDescription;
    wxDateTime m_dtBeg, m_dtEnd, m_dtCreated;
    wxString m_sExecPath;
    wxULongLong m_nVolume;//value in, eg. bytes
    long m_nPriority;
    int m_nGroupId;
    double m_dfDone;
    wxJSONValue m_Params;
    wxGISTaskMessagesArray m_oaMessages;
};

/** \class wxGISTaskEdit task.h
    \brief The edit task class.
*/

class WXDLLIMPEXP_GIS_GP wxGISTaskEdit : 
    public wxGISTask
{
    DECLARE_CLASS(wxGISTaskEdit)
public:
    wxGISTaskEdit(void);
    virtual ~wxGISTaskEdit(void);
    virtual void SetName(const wxString& sName);
    virtual void SetDescription(const wxString& sDescription);
    virtual void SetExecutable(const wxString& sExecutable);
};

/** \class wxGISTaskCategory task.h
    \brief The tasks category class.
*/

class WXDLLIMPEXP_GIS_GP wxGISTaskCategory : 
    public wxGISTaskBase
{
    DECLARE_CLASS(wxGISTaskCategory)
public:
    wxGISTaskCategory(const wxString &sName, wxGISTaskManager* pTaskManager);
    virtual ~wxGISTaskCategory(void);
    virtual void ProcessNetMessage(const wxNetMessage &msg);
    virtual void SetMaxTaskExec(short nMaxExecTaskCount);
    virtual bool CreateTask(wxGISTask* const pTask);
    //NET messages
    virtual void SendNetMessageAsync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
    virtual wxGISNetCommandState SendNetMessageSync(wxGISNetCommand eCmd, wxGISNetCommandState eCmdState, const wxJSONValue &val);
protected:
    virtual void NetCommand(const wxNetMessage &msg);
    virtual void NetNote(const wxNetMessage &msg);
protected:
    wxGISTaskManager* m_pTaskManager;
    short m_nMaxTasks;
};

    //wxGISEnumReturnType AddTask(const wxString &sCategory, int nMaxExecTaskCount, const wxString &sName, const wxString &sDesc, const wxString &sPath, long nPriority, const wxXmlNode *pParameters, wxString & sMsg);
    //wxGISTaskMap GetTasks(void);
    //wxGISTask* const GetTask(int nTaskId);
    //wxGISEnumReturnType DeleteTask(int nTaskId, wxString & sMsg);
    //wxGISEnumReturnType StartTask(int nTaskId, wxString & sMsg);
    //wxGISEnumReturnType StopTask(int nTaskId, wxString & sMsg);
    //wxGISEnumReturnType ChangeTaskPriority(int nTaskId, long nPriority, wxString & sMsg); TODO:
    
    //int GetRunTaskCount(const wxString& sCat);
    //void AddTask(const wxXmlNode* pIniNode);
    //void DeleteTask(const wxXmlNode* pIniNode);
    //void ChangeTask(const wxXmlNode* pIniNode);
    //void UpdateVolume(const wxXmlNode* pIniNode);
    //void UpdatePercent(const wxXmlNode* pIniNode);
    //void AddMessage(const wxXmlNode* pIniNode);
    //void LoadTasks(const wxXmlNode* pTasksNode);
