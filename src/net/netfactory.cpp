/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/net/netfactory.h"
#include "wxgis/net/netevent.h"

//-----------------------------------------------------------------------------
// INetConnFactory
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(INetConnFactory, wxEvtHandler)

INetConnFactory::~INetConnFactory(void)
{
}

bool INetConnFactory::StartServerSearch()
{
    return true;
}

bool INetConnFactory::CanStartServerSearch()
{
    return false;
}

bool INetConnFactory::StopServerSearch()
{
    return true;
}

bool INetConnFactory::CanStopServerSearch()
{
    return false;
}

bool INetConnFactory::IsServerSearching()
{
    return false;
}

wxString INetConnFactory::GetName() const 
{
    return wxEmptyString;
}

void INetConnFactory::Serialize(wxXmlNode* pConfigNode, bool bSave)
{
}

wxGISNetClientConnection* const INetConnFactory::GetConnection(const wxJSONValue &oProperties)
{
    return NULL;
}


//#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
//WX_DEFINE_USER_EXPORTED_OBJARRAY(wxNetConnFactoryArray);

