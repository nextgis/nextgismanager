/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxCatalogUI main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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

#include "wxgis/base.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/framework/framework.h"

#include <wx/dnd.h>
#include <wx/dataobj.h>

#define TREECTRLID	1005 //wxGxCatalog tree
#define LISTCTRLID	1006 //wxGxCatalog contents view
#define TABLECTRLID	1007 //wxGxCatalog table view
#define TABCTRLID	1008 //wxGxCatalog tab with views - contents, preview & etc.
#define MAPCTRLID	1009 //wxGxCatalog map view
//1010 reserved for NOTFIRESELID from catalog.h
#define FILTERCOMBO	1011 //wxGxObjectDialog
#define WXGISHIGHEST	1200 //

/** \class IGxObjectSort catalogui.h
    \brief A Interface class for GxObject sort order in GxObjectContainer.
*/
class IGxObjectSort
{
public:
	virtual ~IGxObjectSort(void){};
	virtual bool IsAlwaysTop(void) = 0;
	virtual bool IsSortEnabled(void) = 0;
};

/** \class IGxObjectWizard catalogui.h
    \brief A Interface class for GxObject wisards.

    If double clicked on gxobject inherited from this interface the Invoke method is executed
*/
class IGxObjectWizard
{
public:
	virtual ~IGxObjectWizard(void){};
	virtual bool Invoke(wxWindow* pParentWnd) = 0;
};

/** \class IGxObjectUI catalogui.h
    \brief A Interface class for GxObject UI.

    This interface class provides some UI onformation (icons, menues, etc.)
*/
class IGxObjectUI
{
public:
	virtual ~IGxObjectUI(void){};
	virtual wxIcon GetLargeImage(void) = 0;
	virtual wxIcon GetSmallImage(void) = 0;
	virtual wxString ContextMenu(void) const = 0;
	virtual wxString NewMenu(void) const = 0;
	//virtual wxDataFormat GetDataFormat(void){return wxDataFormat(wxDF_FILENAME);};
};

/** \class IGxObjectEditUI catalogui.h
    \brief A Interface class for GxObject UI edits.

    This interface class provides edit properties dialog for "Properties" command
*/
class IGxObjectEditUI
{
public:
	virtual ~IGxObjectEditUI(void){};
	virtual void EditProperties(wxWindow *parent){};
};
/*
class IGxSelection
{
public:
	enum wxGISEnumInitiators
	{
		INIT_ALL = -2,
		INIT_NONE = -1
	} Initiator;
	virtual ~IGxSelection(void){};
	virtual void Select( long nObjectID,  bool appendToExistingSelection, long nInitiator ) = 0;
	virtual void Select( long nObjectID ) = 0;
	virtual void Unselect( long nObjectID, long nInitiator) = 0;
	virtual void Clear(long nInitiator) = 0;
	virtual size_t GetCount(void) = 0;
	virtual size_t GetCount(long nInitiator) = 0;
	virtual long GetSelectedObjectID(size_t nIndex) = 0;
	virtual long GetSelectedObjectID(long nInitiator, size_t nIndex) = 0;
	virtual long GetLastSelectedObjectID(void) = 0;
	virtual void SetInitiator(long nInitiator) = 0;
    virtual void Do( long nObjectID ) = 0;
    virtual bool CanRedo() = 0;
	virtual bool CanUndo() = 0;
	virtual void RemoveDo(long nObjectID) = 0;
    virtual long Redo(int nPos = wxNOT_FOUND) = 0;
    virtual long Undo(int nPos = wxNOT_FOUND) = 0;
    virtual void Reset() = 0;
    virtual size_t GetDoSize() = 0;
    virtual int GetDoPos(void) = 0;
    virtual long GetDoID(size_t nIndex) = 0;
    virtual wxArrayLong GetDoArray(void) = 0;
};

class IGxView : public IView
{
public:
	virtual ~IGxView(void){};
	virtual bool Applies(IGxSelection* Selection) = 0;
    virtual void BeginRename(long nObjectID = wxNOT_FOUND) = 0;
};
*/
enum wxGISEnumContentsViewStyle
{
    enumGISCVUndefined = 0,
	enumGISCVReport, 
	enumGISCVSmall,
	enumGISCVLarge,
	enumGISCVList
};

/** \class IGxContentsView catalogui.h
    \brief The IGxContentsView interface class.

    The GxViews which should support Style Changing like switch between report view to list view mast derived from this class.
*/
class IGxContentsView
{
public:
	virtual ~IGxContentsView(void){};
	virtual void SelectAll(void) = 0;
    virtual bool CanSetStyle(void) = 0;
	virtual void SetStyle(wxGISEnumContentsViewStyle style) = 0;
    virtual wxGISEnumContentsViewStyle GetStyle(void) = 0;
};

/** \class IGxDropTarget catalogui.h
    \brief A DropTarget interface class.

    The GxObject which should support drag'n'drop capability mast derived from this class.
*/
class IGxDropTarget
{
public:
	virtual ~IGxDropTarget(void){};
    virtual wxDragResult CanDrop(wxDragResult def) = 0;
    virtual bool Drop(const wxArrayString& GxObjects, bool bMove) = 0;
};

/** \class IGxObjectTreeAttr catalogui.h
    \brief A Interface class for GxObject item view and behaviour in tree.
*/
class IGxObjectTreeAttr
{
public:
	virtual ~IGxObjectTreeAttr(void){};
	virtual bool IsBold(void) = 0;
    virtual wxColour GetColor(void) = 0;
    virtual bool ShowCount(void) = 0;
    virtual size_t GetCount(void) = 0;
};