/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  RxObjectUI classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
/*
#include "wxgis/remoteserverui/rxobjectclientui.h"

//------------------------------------------------------------
// wxRxObjectContainerUI
//------------------------------------------------------------

wxRxObjectContainerUI::wxRxObjectContainerUI(void) : wxRxObjectContainer(), m_pGxPendingUI(NULL)
{
}

wxRxObjectContainerUI::~wxRxObjectContainerUI(void)
{
}

void wxRxObjectContainerUI::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(pChildNode)
	{
		if(pChildNode->GetName().CmpNoCase(wxT("children")) == 0 && pChildNode->HasAttribute(wxT("count")))
			m_nChildCount = wxAtoi(pChildNode->GetAttribute(wxT("count"), wxT("0")));
		if(m_nChildCount == 0)
		{
			m_bIsChildrenLoaded = true;
			return;
		}
		if(pChildNode->GetName().CmpNoCase(wxT("child")) == 0)
		{
            if(m_pGxPendingUI)
				m_pGxPendingUI->OnStopPending();
			//load items
			while(pChildNode)
			{

				wxString sDst = pChildNode->GetAttribute(wxT("dst"), ERR);
				bool bAdd(true);
				for(size_t i = 0; i < m_sDstArray.GetCount(); ++i)
				{
					if(sDst == m_sDstArray[i])
					{
						bAdd = false;
						break;
					}
				}

				wxString sClassName = pChildNode->GetAttribute(wxT("class"), ERR);
				if(!sClassName.IsEmpty() && bAdd)
				{
					IGxObject *pObj = dynamic_cast<IGxObject*>(wxCreateDynamicObject(sClassName));
					IRxObjectClient* pRxObjectCli = dynamic_cast<IRxObjectClient*>(pObj);
					if(pRxObjectCli)
						pRxObjectCli->Init(m_pGxRemoteServer, pChildNode);
					if(!AddChild(pObj))
                    {
						wxDELETE(pObj);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(pObj->GetID());
                    }
					m_sDstArray.Add(sDst);
				}
				pChildNode = pChildNode->GetNext();
			}

            //remove pending
            if(m_pGxPendingUI)
                if(DeleteChild(static_cast<IGxObject*>(m_pGxPendingUI)))
                    m_pGxPendingUI = NULL;

			m_bIsChildrenLoaded = m_nChildCount == m_Children.size();
		    //m_pCatalog->ObjectChanged(this);
		}
	}
}

bool wxRxObjectContainerUI::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return true;
    if(!m_pGxPendingUI)
    {
        m_pGxPendingUI = new wxGxPendingUI();
        if(!AddChild(m_pGxPendingUI))
            wxDELETE(m_pGxPendingUI);
    }
	return wxRxObjectContainer::LoadChildren();
}
*/
