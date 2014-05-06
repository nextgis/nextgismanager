/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/core/core.h"
#include "wxgis/net/netconn.h"

#include <wx/dynarray.h>
#include <wx/event.h>

/** \class INetConnFactory netfactory.h
    \brief The network connection factory interface class.
*/
class WXDLLIMPEXP_GIS_NET INetConnFactory : 
    public wxGISConnectionPointContainer,
    public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(INetConnFactory)
public:
    virtual ~INetConnFactory(void);
	virtual bool StartServerSearch();
    virtual bool CanStartServerSearch();
	virtual bool StopServerSearch();
	virtual bool CanStopServerSearch();
	virtual bool IsServerSearching();
	virtual wxString GetName(void) const;
    virtual wxGISNetClientConnection* const GetConnection(const wxJSONValue &oProperties);
    /** \fn void Serialize(wxXmlNode* pConfigNode, bool bSave)
     *  \brief Store Properties of Factory.
     *  \param pConfigNode wxXmlNode to store params.
     *  \param bSave Set or Get indicator.
     */	 	
    virtual void Serialize(wxXmlNode* pConfigNode, bool bSave = true);

	//virtual char GetID(void) = 0;
	//virtual void SetID(char nID) = 0;
};

//WX_DECLARE_USER_EXPORTED_OBJARRAY(INetConnFactory, wxNetConnFactoryArray, WXDLLIMPEXP_GIS_NET);
WX_DEFINE_ARRAY_WITH_DECL_PTR(INetConnFactory *, wxNetConnFactoryArray, class WXDLLIMPEXP_GIS_NET);
