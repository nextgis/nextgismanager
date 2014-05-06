/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Map Main Commands class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#pragma once
#include "wxgis/mapui/mapui.h"
#include "wxgis/framework/framework.h"

/** \class wxGISMapMainCmd mapcmd.h
    \brief The wxGISMAp main commands class.
*/
class wxGISMapMainCmd :
    public ICommand
{
    DECLARE_DYNAMIC_CLASS(wxGISMapMainCmd)

public:
	wxGISMapMainCmd(void);
	virtual ~wxGISMapMainCmd(void);
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IFrameApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
private:
	IFrameApplication* m_pApp;
	wxIcon m_IconAddLayer;
};