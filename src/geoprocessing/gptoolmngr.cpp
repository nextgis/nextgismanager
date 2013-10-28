/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tools manager.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/core/config.h"
#include "wxgis/core/event.h"

#include "wx/datetime.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGPProcess
///////////////////////////////////////////////////////////////////////////////

wxGPProcess::wxGPProcess(wxString sCommand, wxArrayString saParams, IProcessParent* pParent, ITrackCancel* pTrackCancel) : wxGISProcess(sCommand, saParams, pParent), m_pProgressor(NULL)
{
    m_pTrackCancel = pTrackCancel;
	if(m_pTrackCancel)
		m_pProgressor = m_pTrackCancel->GetProgressor();
}

wxGPProcess::~wxGPProcess(void)
{
}

//void wxGPProcess::OnTerminate(int status)
//{
//    wxGISProcess::OnTerminate(status);
//	if(m_pProgressor)
//		m_pProgressor->SetValue(100);
//}

void wxGPProcess::ProcessInput(wxString sInputData)
{
	if(m_nState != enumGISTaskWork)
        return;
    sInputData = sInputData.Trim(true).Trim(false);
//INFO, DONE, ERR, ??
	wxString sRest;
	if( sInputData.StartsWith(wxT("DONE: "), &sRest) )
	{
		int nPercent = wxAtoi(sRest.Trim(true).Trim(false).Truncate(sRest.Len() - 1));
		wxTimeSpan Elapsed = wxDateTime::Now() - m_dtBeg;//time left
        wxString sTxt;
		if(m_pProgressor)
			m_pProgressor->SetValue(nPercent);

        double nPercentR = 100 - nPercent;
		if(nPercentR >= 0)
		{
			//wxTimeSpan Remains = Elapsed * (nPercentR / nPercent);
			long dMSec = double(Elapsed.GetMilliseconds().ToDouble() * nPercentR) / nPercent;
			wxTimeSpan Remains = wxTimeSpan(0,0,0,dMSec);
			m_dtEstEnd = wxDateTime::Now() + Remains;
			sTxt = wxString(_("Remains ")) + Remains.Format(_("%H hour(s) %M min. %S sec."));
		}

		if(m_pTrackCancel && !sTxt.IsEmpty())
			m_pTrackCancel->PutMessage(sTxt, -1, enumGISMessageTitle);
		return;
	}
	if( sInputData.StartsWith(wxT("INFO: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageNorm);
		return;
	}
	if( sInputData.StartsWith(wxT("ERR: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageErr);
		return;
	}
	if( sInputData.StartsWith(wxT("WARN: "), &sRest) )
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sRest, -1, enumGISMessageWarning);
		return;
	}
    else
	{
		if(m_pTrackCancel)
			m_pTrackCancel->PutMessage(sInputData, -1, enumGISMessageInfo);
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolManager
///////////////////////////////////////////////////////////////////////////////

wxGISGPToolManager::wxGISGPToolManager(void) : m_nRunningTasks(0)
{
	m_bIsOk = false;
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

    m_nMaxTasks = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/max_tasks")), wxThread::GetCPUCount());
#ifdef __WXMSW__
    m_sGeoprocessPath = oConfig.Read(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), wxString(wxT("wxGISGeoprocess.exe")));
#else
    m_sGeoprocessPath = oConfig.Read(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), wxString(wxT("/usr/local/bin/wxgis/wxGISGeoprocess")));
#endif
	wxXmlNode *pToolsNode = oConfig.GetConfigNode(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools")));
	if(!pToolsNode)
		return;

	wxXmlNode *pChild = pToolsNode->GetChildren();
    while (pChild)
    {
        wxString sName = pChild->GetAttribute(wxT("object"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }

        int nCount = wxAtoi(pChild->GetAttribute(wxT("count"), wxT("0")));
        wxString sCmdName = pChild->GetAttribute(wxT("name"), NONAME);

	    wxObject *pObj = wxCreateDynamicObject(sName);
        IGPToolSPtr pRetTool(dynamic_cast<IGPTool*>(pObj));
	    if(pRetTool)
	    {
            sCmdName = pRetTool->GetName();
			TOOLINFO info = {sName, nCount, pRetTool};
			m_ToolsMap[sCmdName] = info;
			m_ToolsPopularMap.insert(std::make_pair(nCount, sCmdName));
        }
        else
            wxLogError(_("Create %s object failed!"), sName.c_str());
        pChild = pChild->GetNext();
    }
	m_bIsOk = true;
}

wxGISGPToolManager::~wxGISGPToolManager(void)
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/max_tasks")), m_nMaxTasks);
		oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools/gp_exec")), m_sGeoprocessPath);

        //read tasks
	    wxXmlNode *pToolsNode = oConfig.GetConfigNode(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tools")));
	    if(pToolsNode)
	    {
		    oConfig.DeleteNodeChildren(pToolsNode);
		    for(std::map<wxString, TOOLINFO>::const_iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
		    {
			    if(pos->second.sClassName.IsEmpty())
				    continue;
			    wxXmlNode* pNewNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("tool")));
			    pNewNode->AddAttribute(wxT("object"), pos->second.sClassName);
			    pNewNode->AddAttribute(wxT("name"), pos->first);
			    pNewNode->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), pos->second.nCount));
                pToolsNode->AddChild(pNewNode);
		    }
	    }
	}

    //kill all processes
    for(size_t i = 0; i < m_ProcessArray.size(); ++i)
    {
       	m_ProcessArray[i].pProcess->Cancel();
        wxDELETE(m_ProcessArray[i].pProcess);
    }
    //delete all existed tools
    //for(std::map<wxString, TOOLINFO>::iterator pos = m_ToolsMap.begin(); pos != m_ToolsMap.end(); ++pos)
    //    wxDELETE(pos->second.pTool);

}

IGPToolSPtr wxGISGPToolManager::GetTool(wxString sToolName, IGxCatalog* pCatalog)
{
    if(sToolName.IsEmpty())
        return IGPToolSPtr();
    std::map<wxString, TOOLINFO>::const_iterator it = m_ToolsMap.find(sToolName);
    if(it == m_ToolsMap.end())
        return IGPToolSPtr();

    wxObject *pObj = wxCreateDynamicObject(it->second.sClassName);
    IGPToolSPtr pRetTool(dynamic_cast<IGPTool*>(pObj));
    if(it->second.pTool)
    {
        if(!it->second.pTool->GetCatalog())
            it->second.pTool->SetCatalog(pCatalog);

        if(pRetTool)
        {
            pRetTool->Copy(it->second.pTool.get());
            m_ToolsMap[sToolName].pTool = pRetTool;
            return pRetTool;
        }
        else
            return IGPToolSPtr();
    }

    if(!pRetTool)
    {
        TOOLINFO inf = {wxEmptyString, 0, IGPToolSPtr()};
        m_ToolsMap[sToolName] = inf;
        return IGPToolSPtr();
    }
    pRetTool->SetCatalog(pCatalog);
    m_ToolsMap[sToolName].pTool = pRetTool;
	return pRetTool;
}

int wxGISGPToolManager::Execute(IGPToolSPtr pTool, ITrackCancel* pTrackCancel)
{
    if(!pTool)
        return false;
    //get tool name and add it to stat of exec
    wxString sToolName = pTool->GetName();
    m_ToolsMap[sToolName].nCount++;

    wxString sToolParams = pTool->GetAsString();
    sToolParams.Replace(wxT("\""), wxT("\\\""));

	//wxString sCommand = wxString::Format(wxT("%s -n "), m_sGeoprocessPath.c_str()) + sToolName + wxT(" -p \"") + sToolParams + wxT("\"");

	wxFileName FName(m_sGeoprocessPath);
    wxArrayString saParams;
    saParams.Add(FName.GetName());
    saParams.Add(wxString(wxT("-n")));
    saParams.Add(sToolName);
    saParams.Add(wxString(wxT("-p")));
    saParams.Add(sToolParams);

    WXGISEXECDDATA data = {new wxGPProcess(m_sGeoprocessPath, saParams, static_cast<IProcessParent*>(this), pTrackCancel), pTool, pTrackCancel};
    m_ProcessArray.push_back(data);
    int nTaskID = m_ProcessArray.size() - 1;
    AddPriority(nTaskID, nTaskID);

	StartProcess(nTaskID);

    return nTaskID;
}

bool wxGISGPToolManager::ExecTask(WXGISEXECDDATA &data, size_t nIndex)
{
    wxDateTime begin = wxDateTime::Now();
    if(data.pTrackCancel)
    {
        data.pTrackCancel->PutMessage(wxString::Format(_("Executing (%s)"), data.pTool->GetName().c_str()), -1, enumGISMessageInfo);
        //add some tool info
        GPParameters Params = data.pTool->GetParameterInfo();
        data.pTrackCancel->PutMessage(wxString(_("Parameters:")), -1, enumGISMessageInfo);
		for(size_t i = 0; i < Params.GetCount(); ++i)
        {
            IGPParameter* pParam = Params[i];
            if(!pParam)
                continue;
            wxString sParamName = pParam->GetName();
            wxString sParamValue = pParam->GetValue();
            data.pTrackCancel->PutMessage(wxString::Format(wxT("%s = %s"), sParamName.c_str(), sParamValue.c_str()), -1, enumGISMessageNorm);
        }
        data.pTrackCancel->PutMessage(wxString::Format(_("Start Time: %s"), begin.Format().c_str()), -1, enumGISMessageInfo);
    }

    data.pProcess->Start();
    m_nRunningTasks++;

	wxGISProcessEvent event(wxPROCESS_START, nIndex);
	PostEvent(event);

    return true;
}

void wxGISGPToolManager::OnFinish(IProcess* pProcess, bool bHasErrors)
{
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pProcess == m_ProcessArray[nIndex].pProcess)
            break;

    wxDateTime end = wxDateTime::Now();
    if(m_ProcessArray[nIndex].pTrackCancel)
    {
        if(bHasErrors)
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("An error occured while executing %s. Failed to execute (%s)."), m_ProcessArray[nIndex].pTool->GetName().c_str(), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageErr);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Error!"), -1, enumGISMessageTitle);
        }
        else
        {
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("Executed (%s) successfully"), m_ProcessArray[nIndex].pTool->GetName().c_str()), -1, enumGISMessageInfo);
            m_ProcessArray[nIndex].pTrackCancel->PutMessage(_("Done"), -1, enumGISMessageTitle);
        }

        wxTimeSpan span = end - m_ProcessArray[nIndex].pProcess->GetBeginTime();
        m_ProcessArray[nIndex].pTrackCancel->PutMessage(wxString::Format(_("End Time: %s (Elapsed Time: %s)"), end.Format().c_str(), span.Format(_("%H hours %M min. %S sec.")).c_str()), -1, enumGISMessageInfo);
    }

	wxGISProcessEvent event(wxPROCESS_FINISH, nIndex, bHasErrors);
	PostEvent(event);

	RunNextTask();
}

void wxGISGPToolManager::RunNextTask(void)
{
    //remove from array
    //m_ProcessArray.erase(m_ProcessArray.begin() + nIndex);
    //reduce counter of runnig threads
    m_nRunningTasks--;
	if(m_nRunningTasks < 0)
		m_nRunningTasks = 0;

    //run not running tasks
    if(m_nRunningTasks < m_nMaxTasks)
    {
		size_t nPrioIndex = GetPriorityTaskIndex();
		if(nPrioIndex >= 0 && nPrioIndex < m_ProcessArray.size())
			ExecTask(m_ProcessArray[nPrioIndex], nPrioIndex);
    }
}


size_t wxGISGPToolManager::GetToolCount()
{
    return m_ToolsMap.size();
}

wxString wxGISGPToolManager::GetPopularTool(size_t nIndex)
{
    std::multimap<int, wxString>::iterator i = m_ToolsPopularMap.begin();
    std::advance(i, nIndex);
    if(i != m_ToolsPopularMap.end())
        return i->second;
    return wxEmptyString;
}

//WXGISEXECDDATA wxGISGPToolManager::GetTask(size_t nIndex)
//{
//    return m_ProcessArray[nIndex];
//}

wxGISEnumTaskStateType wxGISGPToolManager::GetProcessState(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetState();
}

void wxGISGPToolManager::StartProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
    if(m_nRunningTasks < m_nMaxTasks)
        ExecTask(m_ProcessArray[nIndex], nIndex);
	else
	{
		m_ProcessArray[nIndex].pProcess->SetState(enumGISTaskQuered);
		wxGISProcessEvent event(wxPROCESS_STATE_CHANGED, nIndex);
		PostEvent(event);
	}
}

void wxGISGPToolManager::CancelProcess(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	m_ProcessArray[nIndex].pProcess->Cancel();

	wxGISProcessEvent event(wxPROCESS_CANCELED, nIndex);
	PostEvent(event);

	RunNextTask();
}

wxDateTime wxGISGPToolManager::GetProcessStart(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetBeginTime();
}

wxDateTime wxGISGPToolManager::GetProcessFinish(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pProcess->GetEndTime();
}

int wxGISGPToolManager::GetProcessPriority(size_t nIndex)
{
    for(size_t i = 0; i < m_aPriorityArray.size(); ++i)
        if(m_aPriorityArray[i].nIndex == nIndex)
            return m_aPriorityArray[i].nPriority;
	return m_ProcessArray.size();
}

void wxGISGPToolManager::SetProcessPriority(size_t nIndex, int nPriority)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());

    for(size_t i = 0; i < m_aPriorityArray.size(); ++i)
    {
        if(m_aPriorityArray[i].nIndex == nIndex)
        {
            m_aPriorityArray.erase(m_aPriorityArray.begin() + i);
            return AddPriority(nIndex, nPriority);
        }
    }
}

int wxGISGPToolManager::GetPriorityTaskIndex()
{
	int nPriorityIndex = m_ProcessArray.size();
    for(size_t nIndex = 0; nIndex < m_aPriorityArray.size(); nIndex++)
    {
        if(m_ProcessArray[m_aPriorityArray[nIndex].nIndex].pProcess && m_ProcessArray[m_aPriorityArray[nIndex].nIndex].pProcess->GetState() == enumGISTaskQuered)
        {
            nPriorityIndex = m_aPriorityArray[nIndex].nIndex;
            break;
        }
    }
	return nPriorityIndex;
}

void wxGISGPToolManager::AddPriority(int nIndex, int nPriority)
{
    TASKPRIOINFO info = {nIndex, nPriority};
    bool bIncrease = false;
	for(size_t j = 0; j < m_aPriorityArray.size(); j++)
	{
		if(!bIncrease && m_aPriorityArray[j].nPriority >= nPriority)
        {
            m_aPriorityArray.insert(m_aPriorityArray.begin() + j, info);
            bIncrease = true;
            continue;
        }
        if(bIncrease)
        {
            if(m_aPriorityArray[j].nPriority - m_aPriorityArray[j - 1].nPriority < 1)
                m_aPriorityArray[j].nPriority++;
            else
                return;;
        }
	}
    if(!bIncrease)
        m_aPriorityArray.push_back(info);
}

IGPToolSPtr wxGISGPToolManager::GetProcessTool(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < m_ProcessArray.size());
	return m_ProcessArray[nIndex].pTool;
}
