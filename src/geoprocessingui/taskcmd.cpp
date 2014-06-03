/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Task Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/geoprocessingui/taskcmd.h"
#include "wxgis/geoprocessing/geoprocessing.h"
//#include "wxgis/geoprocessingui/gptaskexecdlg.h"
//#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/state.xpm"

//	0	Show task config dialog
//  1   Show task execution dialog
//  2   Start
//  3   Stop
//  4   Show exec process
//  5   ?

//----------------------------------------------------------------------------
// wxGISTaskCmd
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISTaskCmd, wxGISCommand)

wxGISTaskCmd::wxGISTaskCmd(void) : wxGISCommand()
{
}

wxGISTaskCmd::~wxGISTaskCmd(void)
{
}

wxIcon wxGISTaskCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 2:
            if(!m_IconStart.IsOk())
            {
                wxImageList ImageList(16, 16, true, 10);
                ImageList.Add(wxBitmap(state_xpm));
                m_IconStart = ImageList.GetIcon(4);
            }
            return m_IconStart;
		case 3:
            if(!m_IconStop.IsOk())
            {
                wxImageList ImageList(16, 16, true, 10);
                ImageList.Add(wxBitmap(state_xpm));
                m_IconStop = ImageList.GetIcon(7);
            }
            return m_IconStop;
		case 1:
            if(!m_IconInfo.IsOk())
            {
                wxImageList ImageList(16, 16, true, 10);
                ImageList.Add(wxBitmap(state_xpm));
                m_IconInfo = ImageList.GetIcon(0);
            }
            return m_IconInfo;
		case 0:
		default:
			return wxNullIcon;
	}
}

wxString wxGISTaskCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool dialog"));
		case 1:
			return wxString(_("Show execution dialog"));
		case 2:
			return wxString(_("Start task"));
		case 3:
			return wxString(_("Stop task"));
		default:
		return wxEmptyString;
	}
}

wxString wxGISTaskCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return wxString(_("Geoprocessing"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISTaskCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 1:
		case 0:
		case 2:
		case 3:
		default:
	        return false;
	}
	return false;
}

bool wxGISTaskCmd::GetEnabled(void)
{
    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

    switch(m_subtype)
	{
		case 0: //Show task config dialog
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if(!pGxTask)
                        return false;
                    if(pGxTask->GetState() != enumGISTaskWork)
                        return true;
                }
            }
            return false;
		case 1: //Show task execution dialog
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if(!pGxTask)
                        return false;
                    if(pGxTask->GetState() == enumGISTaskWork || pGxTask->GetState() == enumGISTaskDone || pGxTask->GetState() == enumGISTaskError)
                        return true;
                }
            }
            return false;
		case 2: //Start task
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if(!pGxTask)
                        return false;
                    return !(pGxTask->GetState() == enumGISTaskQuered || pGxTask->GetState() == enumGISTaskWork);
                    //if(pGxTask->GetState() != enumGISTaskQuered && pGxTask->GetState() != enumGISTaskWork)
                    //    return true;
                }
            }
            return false;
		case 3: //Stop task
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if(!pGxTask)
                        return false;
                    return !(pGxTask->GetState() == enumGISTaskPaused || pGxTask->GetState() == enumGISTaskError || pGxTask->GetState() == enumGISTaskDone);
                    //if(pGxTask->GetState() != enumGISTaskPaused)
                    //    return true;
                }
            }
            return false;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISTaskCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISTaskCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool config dialog"));
		case 1:
			return wxString(_("Show task execution dialog"));
		case 2:
			return wxString(_("Start task"));
		case 3:
			return wxString(_("Stop task"));
		default:
			return wxEmptyString;
	}
}

void wxGISTaskCmd::OnClick(void)
{
    wxGxSelection* pSel = m_pApp->GetGxSelection();
    wxGxCatalogBase* pCat = GetGxCatalog();

	switch(m_subtype)
	{
		case 0:
            if(pCat && pSel)
            {
                //for(size_t i = 0; i < pSel->GetCount(); ++i)
                //{
                //    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                //    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                //    if(pGxTask)
                //        pGxTask->ShowToolConfig(dynamic_cast<wxWindow*>(m_pApp));
                //}
            }
			break;
		case 1:
            if(pCat && pSel)
            {
                //for(size_t i = 0; i < pSel->GetCount(); ++i)
                //{
                //    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                //    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                //    if(pGxTask)
                //        pGxTask->ShowProcess(dynamic_cast<wxWindow*>(m_pApp));
                //}
            }
			break;
		case 2:
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if (pGxTask)
                    {
                        pGxTask->StartTask();
//                        wxTheApp->Yield();
                    }
                }
            }
			break;
		case 3:
            if(pCat && pSel)
            {
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject);
                    if (pGxTask)
                    {
                        pGxTask->StopTask();
//                        wxTheApp->Yield();
                    }
                }
            }
			break;
		default:
			return;
	}
}

bool wxGISTaskCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = dynamic_cast<wxGxApplication*>(pApp);
	return true;
}

wxString wxGISTaskCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool config dialog"));
		case 1:
			return wxString(_("Show task execution dialog"));
		case 2:
			return wxString(_("Start task"));
		case 3:
			return wxString(_("Stop task"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISTaskCmd::GetCount(void)
{
	return 4;
}
