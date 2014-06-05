/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommonCmd class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012 Dmitry Barishnikov
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

#include "wxgis/framework/framework.h"

#define NO_CATEGORY wxString(_("[No category]"))

class WXDLLIMPEXP_GIS_FRW wxGISApplicationBase;
/** \enum wxGISEnumCommandKind
    \brief A command types.

    This is predefined command types.
*/
enum wxGISEnumCommandKind
{
	enumGISCommandSeparator = wxITEM_SEPARATOR, /**< The type is separator */ 
	enumGISCommandNormal = wxITEM_NORMAL,       /**< The type is normal command */
	enumGISCommandCheck = wxITEM_CHECK,         /**< The type is check command */
	enumGISCommandRadio = wxITEM_RADIO,         /**< The type is radio */
	enumGISCommandMax = wxITEM_MAX,             /**< The type is max available command from wxWidgets (the less command have no analogs in wxWidgets)*/
	enumGISCommandMenu = wxITEM_MAX + 10,       /**< The type is menu */
	enumGISCommandControl,                      /**< The type is control */
    enumGISCommandDropDown                      /**< The type is drop down */
};

/** \class wxGISCommand command.h
    \brief A wxGISCommand base abstract class for commnads.

    This is base class for command.    
*/

class WXDLLIMPEXP_GIS_FRW wxGISCommand : public wxObject
{
    //DECLARE_ABSTRACT_CLASS(wxGISCommand)
public:
    /** \fn wxGISCommand(void)
     *  \brief A constructor.
     */	
    wxGISCommand(void);
    /** \fn virtual ~wxGISCommand(void)
     *  \brief A destructor.
     */
    virtual ~wxGISCommand(void);
	//pure virtual
	virtual wxIcon GetBitmap(void) = 0;
	virtual wxString GetCaption(void) = 0;
	virtual wxString GetCategory(void) = 0;
	virtual bool GetChecked(void) = 0;
	virtual bool GetEnabled(void) = 0;
	virtual wxString GetMessage(void) = 0;
	virtual wxGISEnumCommandKind GetKind(void) = 0;
	virtual void OnClick(void) = 0;
	virtual bool OnCreate(wxGISApplicationBase* pApp) = 0;
	virtual wxString GetTooltip(void) = 0;
	virtual unsigned char GetCount(void) = 0;
	virtual void SetId(long nId);
	virtual long GetId(void) const;
	//
	virtual void SetSubType(unsigned char SubType);
	virtual unsigned char GetSubType(void) const;
protected:
	unsigned char m_subtype;
	long m_CommandId;
};

/** \def vector<wxGISCommand*> COMMANDARRAY
    \brief A wxGISCommand array.
*/
WX_DEFINE_ARRAY_PTR(wxGISCommand*, wxCommandPtrArray);

class IToolBarControl
{
public:
    /** \fn virtual ~IToolBarControl(void)
     *  \brief A destructor.
     */
    virtual ~IToolBarControl(void) {};
	virtual void Activate(wxGISApplicationBase* pApp) = 0;
	virtual void Deactivate(void) = 0;
};

/** \class IToolControl command.h
    \brief A IToolControl interface class.

    This is base class for tool control.    
*/
class IToolControl
{
public:
    /** \fn virtual ~IToolControl(void)
     *  \brief A destructor.
     */
    virtual ~IToolControl(void) {};
	virtual IToolBarControl* GetControl(void) = 0;
	virtual wxString GetToolLabel(void) = 0;
	virtual bool HasToolLabel(void) = 0;
};

/** \class ITool command.h
    \brief A ITool interface class.

    This is base class for tool.    
*/
class ITool :
	public wxGISCommand
{
public:
    /** \fn virtual ~ITool(void)
     *  \brief A destructor.
     */
    virtual ~ITool(void) {};
	virtual void SetChecked(bool bCheck) = 0;
	virtual wxCursor GetCursor(void) = 0;
	virtual void OnMouseDown(wxMouseEvent& event) = 0;
	virtual void OnMouseUp(wxMouseEvent& event) = 0;
	virtual void OnMouseMove(wxMouseEvent& event) = 0;
	virtual void OnMouseDoubleClick(wxMouseEvent& event) = 0;
};

/** \class IDropDownCommand command.h
    \brief A IDropDownCommand interface class.

    This is base class for drop down command.    
*/
class IDropDownCommand
{
public:
    /** \fn virtual ~IDropDownCommand(void)
     *  \brief A destructor.
     */
    virtual ~IDropDownCommand(void) {};
    /** \fn virtual wxMenu* GetDropDownMenu(void)
     *  \brief Return DropDown Menu to show under button.
     *  \return The menu pointer
     */
	virtual wxMenu* GetDropDownMenu(void) = 0;
    /** \fn virtual void OnDropDownCommand(int nID)
     *  \brief Execute the DropDown menu command
     *  \param nID The command ID to execute. The command ID will be from ID_MENUCMD to 2049. See #wxGISPluginIDs
     */	
    virtual void OnDropDownCommand(int nID) = 0;
};

class wxGISCommonCmd :
	public wxGISCommand
{
	DECLARE_DYNAMIC_CLASS(wxGISCommonCmd)
public:
	wxGISCommonCmd(void);
	virtual ~wxGISCommonCmd(void);
	//wxGISCommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(wxGISApplicationBase* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
private:
	wxGISApplicationBase* m_pApp;
	wxIcon m_IconOpt;
};

