/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationBase class. Base application functionality (commands, menues, etc.)
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Dmitry Baryshnikov
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
#include "wxgis/framework/applicationbase.h"
#include "wxgis/core/config.h"

#include "gdal_priv.h"

//--------------------------------------------------------------------------
// wxGISApplicationBase
//--------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISApplicationBase, IApplication)

wxGISApplicationBase::wxGISApplicationBase()
{
     m_CurrentTool = NULL;
     m_pMenuBar = NULL;
}

wxGISApplicationBase::~wxGISApplicationBase()
{
	//delete oposite direction to prevent delete sub menues // refcount!
	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
	    wsDELETE(m_CommandBarArray[i]);

	for(size_t i = 0; i < m_CommandArray.size(); ++i)
    {
        wxGISPointer* pPointer = dynamic_cast<wxGISPointer*>(m_CommandArray[i]);
        if(pPointer)
        {
            pPointer->Release();
            m_CommandArray[i] = NULL;
            //wsDELETE(m_CommandArray[i]);
        }
        else
            wxDELETE(m_CommandArray[i]);
    }
}

wxGISCommand* wxGISApplicationBase::GetCommand(long CmdID) const
{
    for (size_t i = 0; i < m_CommandArray.size(); ++i)
    {
        if (m_CommandArray[i] && m_CommandArray[i]->GetId() == CmdID)
        {
			return m_CommandArray[i];
        }
    }
	return NULL;
}

wxGISCommandBar* wxGISApplicationBase::GetCommandBar(const wxString &sName) const
{
	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
        if(m_CommandBarArray[i]->GetName() == sName)
			return m_CommandBarArray[i];
	return NULL;
}


wxGISCommandBarPtrArray wxGISApplicationBase::GetCommandBars(void) const
{
	return m_CommandBarArray;
}

wxCommandPtrArray wxGISApplicationBase::GetCommands(void) const
{
	return m_CommandArray;
}

void wxGISApplicationBase::RemoveCommandBar(wxGISCommandBar* pBar)
{
    for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i] == pBar)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBMenubar:
                if(m_pMenuBar)
                    m_pMenuBar->RemoveMenu(pBar);
				break;
			case enumGISCBToolbar:
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
			case enumGISCBNone:
				break;
			}
			wsDELETE(pBar);
			m_CommandBarArray.RemoveAt(i);
			break;
		}
	}
}

bool wxGISApplicationBase::AddCommandBar(wxGISCommandBar* pBar)
{
	if(!pBar)
		return false;
	pBar->Reference();
    m_CommandBarArray.Add(pBar);
	switch(pBar->GetType())
	{
	case enumGISCBMenubar:
        if(m_pMenuBar)
            m_pMenuBar->AddMenu(pBar);
		break;
	case enumGISCBToolbar:
	case enumGISCBContextmenu:
	case enumGISCBSubMenu:
	case enumGISCBNone:
		break;
	}
	return true;
}

wxGISCommand* wxGISApplicationBase::GetCommand(const wxString &sCmdName, unsigned char nCmdSubType) const
{
    for(size_t i = 0; i < m_CommandArray.GetCount(); ++i)
	{
		wxClassInfo * pInfo = m_CommandArray[i]->GetClassInfo();
		wxString sCommandName = pInfo->GetClassName();
		if(sCommandName == sCmdName && m_CommandArray[i]->GetSubType() == nCmdSubType)
			return m_CommandArray[i];
	}
	return NULL;
}

void wxGISApplicationBase::OnMouseDown(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseDown(event);
}

void wxGISApplicationBase::OnMouseUp(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseUp(event);
}

void wxGISApplicationBase::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseDoubleClick(event);
}

void wxGISApplicationBase::OnMouseMove(wxMouseEvent& event)
{
	if(m_CurrentTool)
		m_CurrentTool->OnMouseMove(event);
}

wxGISMenuBar* wxGISApplicationBase::GetGISMenuBar(void) const
{
	return m_pMenuBar;
}


void wxGISApplicationBase::RegisterChildWindow(wxWindowID nWndID)
{
    if(m_anWindowsIDs.Index(nWndID) == wxNOT_FOUND)
        m_anWindowsIDs.Add(nWndID);
}

void wxGISApplicationBase::UnRegisterChildWindow(wxWindowID nWndID)
{
    int nIndex = m_anWindowsIDs.Index(nWndID);
    if(nIndex == wxNOT_FOUND)
        return;
    m_anWindowsIDs.RemoveAt(nIndex);
}

wxWindow* wxGISApplicationBase::GetRegisteredWindowByType(const wxClassInfo * info) const
{
    for(size_t i = 0; i < m_anWindowsIDs.GetCount(); ++i)
    {
        wxWindow* pWnd = wxWindow::FindWindowById(m_anWindowsIDs[i]);
        if(pWnd && pWnd->IsKindOf(info))
            return pWnd;       
    }
    return NULL;
}

void wxGISApplicationBase::LoadCommands(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	unsigned int nCmdCounter(0);
	while(child)
	{
		wxString sName = child->GetAttribute(wxT("name"), wxT(""));
		if(!sName.IsEmpty())
		{
			wxObject *obj = wxCreateDynamicObject(sName);
			wxGISCommand *pCmd = dynamic_cast<wxGISCommand*>(obj);//wxDynamicCast(obj, wxGISCommand);//
			if(pCmd)
			{
				for(unsigned char i = 0; i < pCmd->GetCount(); i++)
				{
					wxObject *newobj = wxCreateDynamicObject(sName);
					wxGISCommand *pNewCmd = dynamic_cast<wxGISCommand*>(newobj);//wxDynamicCast(newobj, wxGISCommand);//
					if(pNewCmd && pNewCmd->OnCreate(this))
					{
						pNewCmd->SetId(ID_PLUGINCMD + nCmdCounter);
						pNewCmd->SetSubType(i);
						//TODO: check doubles
                        m_CommandArray.Add(pNewCmd);
						nCmdCounter++;
					}
				}
				wxDELETE(pCmd);
			}
		}
		child = child->GetNext();
	}
}

void wxGISApplicationBase::LoadMenues(wxXmlNode* pRootNode)
{
	if(pRootNode)
	{
	//	create all menues and after that serialize each of them?
		wxXmlNode *child = pRootNode->GetChildren();
		while(child)
		{
			bool bAdd = true;
			wxString sName = child->GetAttribute(wxT("name"), wxT(""));
			for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
			{
				if(m_CommandBarArray[i]->GetName() == sName)
				{
					bAdd = false;
					break;
				}
			}
			if(bAdd)
			{
				wxString sCaption = child->GetAttribute(wxT("caption"), wxT("No Title"));
				wxGISMenu* pMenu = new wxGISMenu(sName, sCaption, enumGISCBContextmenu);//sCaption for Title enumGISCBMenubar
				pMenu->Serialize(this, child, false);
				pMenu->Reference();
				m_CommandBarArray.Add(pMenu);
			}
			child = child->GetNext();
		}
	}
}

void wxGISApplicationBase::Command(wxGISCommand* pCmd)
{
    if(NULL == pCmd)
        return;
    ITool* pTool = dynamic_cast<ITool*>(pCmd);
    if (pCmd->GetKind() != enumGISCommandNormal && pTool != NULL)
    {
		//uncheck
		if(m_CurrentTool)
			m_CurrentTool->SetChecked(false);
		//check
		pTool->SetChecked(true);
		m_CurrentTool = pTool;
	}
    else
    {
		pCmd->OnClick();
    }
}

bool wxGISApplicationBase::CreateApp(void)
{
	
    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

    //load GDAL defaults
    wxString sGDALCacheMax = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), wxString(wxT("128")));
    CPLSetConfigOption("GTIFF_REPORT_COMPD_CS", "YES");
    CPLSetConfigOption("GTIFF_ESRI_CITATION", "YES");

    CPLSetConfigOption("GDAL_CACHEMAX", sGDALCacheMax.mb_str());
    CPLSetConfigOption("LIBKML_USE_DOC.KML", "no");
    CPLSetConfigOption("GDAL_USE_SOURCE_OVERVIEWS", "ON");
    CPLSetConfigOption("OSR_USE_CT_GRAMMAR", "FALSE");

    //GDAL_MAX_DATASET_POOL_SIZE
    //OGR_ARC_STEPSIZE


    //load commands
	wxXmlNode* pCommandsNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/commands")));

    if(pCommandsNode)
		LoadCommands(pCommandsNode);

    return true;
}
