/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommonCmd class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2012 Bishop
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
#include "wxgis/framework/command.h"

#include "wxgis/framework/applicationbase.h"

#include "../../art/options.xpm"

//	0	Exit
//	1	About
//	2	Customize
//	3	Separator
//	4	StatusBar
//  5   Options
//  6   ?

//---------------------------------------------------------------------------------
// wxGISCommand
//---------------------------------------------------------------------------------

//IMPLEMENT_ABSTRACT_CLASS(wxGISCommand, wxObject)

wxGISCommand::wxGISCommand(void) : m_subtype(0)
{
}

wxGISCommand::~wxGISCommand(void)
{
}

void wxGISCommand::SetID(long nID)
{
    m_CommandID = nID;
}

long wxGISCommand::GetID(void) const 
{
    return m_CommandID;
}

void wxGISCommand::SetSubType(unsigned char SubType)
{
    m_subtype = SubType;
}

unsigned char wxGISCommand::GetSubType(void) const 
{
    return m_subtype;
}

//---------------------------------------------------------------------------------
// wxGISCommonCmd
//---------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCommonCmd, wxGISCommand)

wxGISCommonCmd::wxGISCommonCmd(void) : wxGISCommand(), m_IconOpt(options_xpm)
{
}

wxGISCommonCmd::~wxGISCommonCmd(void)
{
}

wxIcon wxGISCommonCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 1:
			if(m_pApp)
				return m_pApp->GetAppIcon();
		case 5:	
			return m_IconOpt;
		case 0:	
		case 2:	
		case 3:	
		case 4:	
		default:
			return wxNullIcon;
	}
	return wxNullIcon;
}

wxString wxGISCommonCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("E&xit"));
		case 1:	
			return wxString(_("&About..."));
		case 2:	
			return wxString(_("Customize..."));
		case 3:	
			return wxString(_("Separator"));
		case 4:	
			return wxString(_("StatusBar"));
		case 5:	
			return wxString(_("Options..."));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCommonCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("File"));
		case 1:	
			return wxString(_("Help"));
		case 2:	
		case 4:
			return wxString(_("View"));
		case 5:
			return wxString(_("Tools"));
		case 3:	
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCommonCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
		case 3:	
		case 5:	
		default:
			return false;
		case 4:
			return m_pApp->IsStatusBarShown();
	}
	return false;
}

bool wxGISCommonCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Exit
			return true;
		case 1://About
			return true;
		case 2://Customize
			return true;
		case 3://Separator
			return true;
		case 4://StatusBar
			return true;
		case 5://Options
            return true;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCommonCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Exit
			return enumGISCommandNormal;
		case 1://About
			return enumGISCommandNormal;
		case 2://Customize
			return enumGISCommandNormal;
		case 3://Separator
			return enumGISCommandSeparator;
		case 4://StatusBar
			return enumGISCommandCheck;
		case 5://Options
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCommonCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Exit application"));
		case 1:	
			return wxString(_("About application"));
		case 2:	
			return wxString(_("Customize application"));
		case 3:	
			return wxString(_("Separator"));
		case 4:	
			return wxString(_("Show/Hide Statusbar"));
		case 5:	
			return wxString(_("Application options"));
		default:
			return wxEmptyString;
	}
}

void wxGISCommonCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
		{
			wxFrame* pFrame = dynamic_cast<wxFrame*>(m_pApp);
			if(pFrame)
				pFrame->Close();
			break;
		}
		case 1:
			m_pApp->OnAppAbout();
			break;
		case 2:
			m_pApp->Customize();
			break;
		case 4:
			m_pApp->ShowStatusBar(!m_pApp->IsStatusBarShown());
			break;
		case 5:
			m_pApp->OnAppOptions();
			break;
		case 3:
		default:
			return;
	}
}

bool wxGISCommonCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCommonCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Exit"));
		case 1:	
			return wxString(_("About"));
		case 2:	
			return wxString(_("Customize"));
		case 4:	
			return wxString(_("Show/Hide Statusbar"));
		case 5:	
			return wxString(_("Application options"));
		case 3:	
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCommonCmd::GetCount(void)
{
	return 6;
}
