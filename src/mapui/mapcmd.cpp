/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Map Main Commands class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/mapui/mapcmd.h"

#include "../../art/add_layer.xpm"

//	0	Add layer

IMPLEMENT_DYNAMIC_CLASS(wxGISMapMainCmd, wxObject)

wxGISMapMainCmd::wxGISMapMainCmd(void)
{
}

wxGISMapMainCmd::~wxGISMapMainCmd(void)
{
}

wxIcon wxGISMapMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconAddLayer.IsOk())
				m_IconAddLayer = wxIcon(add_layer_xpm);
			return m_IconAddLayer;
		default:
			return wxNullIcon;
	}
}

wxString wxGISMapMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("&Add Layer"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISMapMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Map"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISMapMainCmd::GetChecked(void)
{
	return false;
}

bool wxGISMapMainCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Add layer
		{
			IMxApplication* pMxApp = dynamic_cast<IMxApplication*>(m_pApp);
			if(pMxApp)
			{
    //            wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				//IGxSelection* pSel = pGxCatalogUI->GetSelection();
				//if(pSel->GetCount() == 0)
				//	return false;
				//IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
				//if(!pGxObject)
				//	return false;
				//IGxObject* pParentGxObject = pGxObject->GetParent();
				//if(!pParentGxObject)
				return true;
			}
			return false;
		}
		default:
			return false;
	}
	return false;
}

wxGISEnumCommandKind wxGISMapMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Add layer
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISMapMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Add layer to map"));
		default:
			return wxEmptyString;
	}
}

void wxGISMapMainCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
			//{
			//	IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			//	if(pGxApp)
			//	{
   //                 wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
			//		IGxSelection* pSel = pGxCatalogUI->GetSelection();
			//		if(pSel->GetCount() == 0)
			//			break;
			//		IGxObjectSPtr pGxObject = pGxCatalogUI->GetRegisterObject(pSel->GetSelectedObjectID(0));
   //                 if(!pGxObject)
   //                     return;
			//		IGxObject* pParentGxObject = pGxObject->GetParent();
			//		if(dynamic_cast<IGxObjectContainer*>(pParentGxObject))
			//		{
			//			pSel->Select(pParentGxObject->GetID(), false, IGxSelection::INIT_ALL);
			//		}
			//		else
			//		{
			//			IGxObject* pGrandParentGxObject = pParentGxObject->GetParent();
			//			pSel->Select(pGrandParentGxObject->GetID(), false, IGxSelection::INIT_ALL);
			//		}
			//	}
			//}
			break;
		default:
			return;
	}
}

bool wxGISMapMainCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISMapMainCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Add Layer"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISMapMainCmd::GetCount(void)
{
	return 1;
}

