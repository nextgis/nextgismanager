/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommonCmd class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2012 Dmitry Baryshnikov
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
#include "wxgis/framework/command.h"

#include "wxgis/framework/applicationbase.h"

#include "../../art/options.xpm"

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

void wxGISCommand::SetId(long nId)
{
    m_CommandId = nId;
}

long wxGISCommand::GetId(void) const
{
    return m_CommandId;
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
		case enumGISCommonCmdAbout:
			if(m_pApp)
				return m_pApp->GetAppIcon();
		case enumGISCommonCmdOptions:
			return m_IconOpt;
		case enumGISCommonCmdExit:
		case enumGISCommonCmdCustomize:
		case enumGISCommonCmdSeparator:
		case enumGISCommonCmdStatusBar:
        case enumGISCommonCmdFitToolbars:
        case enumGISCommonCmdOptimizeToolbars:
		default:
			return wxNullIcon;
	}
	return wxNullIcon;
}

wxString wxGISCommonCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
			return wxString(_("E&xit"));
		case enumGISCommonCmdAbout:
			return wxString(_("&About..."));
		case enumGISCommonCmdCustomize:
			return wxString(_("Customize..."));
		case enumGISCommonCmdSeparator:
			return wxString(_("Separator"));
		case enumGISCommonCmdStatusBar:
			return wxString(_("StatusBar"));
		case enumGISCommonCmdOptions:
			return wxString(_("Options..."));
		case enumGISCommonCmdFitToolbars:
			return wxString(_("&Fit toolbars"));
		case enumGISCommonCmdOptimizeToolbars:
			return wxString(_("&Optimize toolbars"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCommonCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
			return wxString(_("File"));
		case enumGISCommonCmdAbout:
			return wxString(_("Help"));
		case enumGISCommonCmdCustomize:
		case enumGISCommonCmdStatusBar:
        case enumGISCommonCmdFitToolbars:
        case enumGISCommonCmdOptimizeToolbars:
			return wxString(_("View"));
		case enumGISCommonCmdOptions:
			return wxString(_("Tools"));
		case enumGISCommonCmdSeparator:
		default:
			return NO_CATEGORY;
	}
}

bool wxGISCommonCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
		case enumGISCommonCmdAbout:
		case enumGISCommonCmdCustomize:
		case enumGISCommonCmdSeparator:
		case enumGISCommonCmdOptions:
        case enumGISCommonCmdFitToolbars:
        case enumGISCommonCmdOptimizeToolbars:
		default:
			return false;
		case enumGISCommonCmdStatusBar:
			return m_pApp->IsStatusBarShown();
	}
	return false;
}

bool wxGISCommonCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
			return true;
		case enumGISCommonCmdAbout:
			return true;
		case enumGISCommonCmdCustomize:
			return true;
		case enumGISCommonCmdSeparator:
			return true;
		case enumGISCommonCmdStatusBar:
			return true;
		case enumGISCommonCmdOptions:
            return true;
        case enumGISCommonCmdFitToolbars:
            return true;
        case enumGISCommonCmdOptimizeToolbars:
            return true;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCommonCmd::GetKind(void)
{
	switch(m_subtype)
	{
    case enumGISCommonCmdExit:
			return enumGISCommandNormal;
		case enumGISCommonCmdAbout:
			return enumGISCommandNormal;
		case enumGISCommonCmdCustomize:
			return enumGISCommandNormal;
		case enumGISCommonCmdSeparator:
            return enumGISCommandSeparator;
		case enumGISCommonCmdStatusBar:
			return enumGISCommandCheck;
		case enumGISCommonCmdOptions:
			return enumGISCommandNormal;
        case enumGISCommonCmdFitToolbars:
            return enumGISCommandNormal;
        case enumGISCommonCmdOptimizeToolbars:
            return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCommonCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
			return wxString(_("Exit application"));
		case enumGISCommonCmdAbout:
			return wxString(_("About application"));
		case enumGISCommonCmdCustomize:
			return wxString(_("Customize application"));
		case enumGISCommonCmdSeparator:
			return wxString(_("Separator"));
		case enumGISCommonCmdStatusBar:
			return wxString(_("Show/Hide Statusbar"));
		case enumGISCommonCmdOptions:
			return wxString(_("Application options"));
		case enumGISCommonCmdFitToolbars:
			return wxString(_("Fit toolbars"));
		case enumGISCommonCmdOptimizeToolbars:
			return wxString(_("Optimize toolbars"));
		default:
			return wxEmptyString;
	}
}

void wxGISCommonCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case enumGISCommonCmdExit:
		{
			wxFrame* pFrame = dynamic_cast<wxFrame*>(m_pApp);
			if(pFrame)
				pFrame->Close();
			break;
		}
		case enumGISCommonCmdAbout:
			m_pApp->OnAppAbout();
			break;
		case enumGISCommonCmdCustomize:
			m_pApp->Customize();
			break;
		case enumGISCommonCmdStatusBar:
			m_pApp->ShowStatusBar(!m_pApp->IsStatusBarShown());
			break;
		case enumGISCommonCmdOptions:
			m_pApp->OnAppOptions();
			break;
        case enumGISCommonCmdFitToolbars:
            m_pApp->FitToolbars();
            break;
        case enumGISCommonCmdOptimizeToolbars:
            m_pApp->OptimizeToolbars();
            break;
		case enumGISCommonCmdSeparator:
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
		case enumGISCommonCmdExit:
			return wxString(_("Exit"));
		case enumGISCommonCmdAbout:
			return wxString(_("About"));
		case enumGISCommonCmdCustomize:
			return wxString(_("Customize"));
		case enumGISCommonCmdStatusBar:
			return wxString(_("Show/Hide Statusbar"));
		case enumGISCommonCmdOptions:
			return wxString(_("Application options"));
        case enumGISCommonCmdFitToolbars:
			return wxString(_("Fit toolbars"));
        case enumGISCommonCmdOptimizeToolbars:
			return wxString(_("Optimize toolbars"));
		case enumGISCommonCmdSeparator:
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCommonCmd::GetCount(void)
{
	return enumGISCommonCmdMax;
}
