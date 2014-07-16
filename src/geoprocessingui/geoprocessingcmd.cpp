/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Geoprocessing Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Dmitry Baryshnikov
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

#include "wxgis/geoprocessingui/geoprocessingcmd.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/catalogui/processing.h"


#include "../../art/export.xpm"
#include "../../art/toolview.xpm"

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxcontdialog.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/catalogui/processing.h"

#include "wx/busyinfo.h"

//  0   Show/hide toolbox pane
//	1	Export
//	2	Export with parameters
//  3   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISGeoprocessingCmd, wxGISCommand)

wxGISGeoprocessingCmd::wxGISGeoprocessingCmd(void) : wxGISCommand()
{
    m_pToolboxView = NULL;
}

wxGISGeoprocessingCmd::~wxGISGeoprocessingCmd(void)
{
}

wxIcon wxGISGeoprocessingCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if (!m_IconToolView.IsOk())
				m_IconToolView = wxIcon(toolview_xpm);
			return m_IconToolView;
		case 1:
		case 2:
			if(!m_IconGPMenu.IsOk())
				m_IconGPMenu = wxIcon(export_xpm);
			return m_IconGPMenu;
		default:
			return wxNullIcon;
	}
}

wxString wxGISGeoprocessingCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show/Hide &Toolbox panel"));
		case 1:
			return wxString(_("&Export"));
		case 2:
			return wxString(_("E&xport with parameters"));
		default:
		    return wxEmptyString;
	}
}

wxString wxGISGeoprocessingCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("View"));
		case 1:
		case 2:
			return wxString(_("Geoprocessing"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISGeoprocessingCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 0:
            return m_pApp->IsApplicationWindowShown(m_pToolboxView);
		case 1:
		case 2:
		default:
	        return false;
	}
	return false;
}

bool wxGISGeoprocessingCmd::GetEnabled(void)
{
	wxCHECK_MSG(m_pGxApp, false, wxT("Application pointer is null"));

	if (NULL == m_pToolboxView)
	{
		m_pToolboxView = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxAxToolboxView));
	}

	wxGxSelection* pSel = m_pGxApp->GetGxSelection();
	wxGxCatalogBase* pCat = GetGxCatalog();


    switch(m_subtype)
	{
		case 0://Show/Hide Toolbox panel
            return NULL != m_pToolboxView;
		case 1://Export
			if (NULL != pSel && NULL != pCat)
			{
				for (size_t i = 0; i < pSel->GetCount(); ++i)
				{
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if (NULL != pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)) || NULL != pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
					{
						return true;
					}
				}
			}
			return false;
		case 2://Export wit parameters - only one file accepted
			if (NULL != pSel && NULL != pCat)
			{
				for (size_t i = 0; i < pSel->GetCount(); ++i)
				{
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					if (NULL != pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
					{
						return true;
					}
				}
			}
			return false;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISGeoprocessingCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Show/hide toolbox panel
            return enumGISCommandCheck;
		case 1://Export
		case 2://Export with parameters
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISGeoprocessingCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show/Hide Toolbox panel"));
		case 1:
			return wxString(_("Export selected item(s) to another format"));
		case 2:
			return wxString(_("Export selected item to another format"));
		default:
			return wxEmptyString;
	}
}

void wxGISGeoprocessingCmd::OnClick(void)
{
	wxCHECK_RET(m_pGxApp && m_pApp, wxT("Application pointer is null"));

	wxGxSelection* pSel = m_pGxApp->GetGxSelection();
	wxGxCatalogBase* pCat = GetGxCatalog();

	switch (m_subtype)
	{
	case 0:
		m_pApp->ShowApplicationWindow(m_pToolboxView, !m_pApp->IsApplicationWindowShown(m_pToolboxView));
        break;
	case 1:
		if (NULL != pSel && NULL != pCat)
		{
			//1. fill the IGxDataset* array
            wxVector<IGxDataset*> paDatasets;
			for (size_t i = 0; i < pSel->GetCount(); ++i)
			{
				wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
				if (NULL != pGxObject)
				{
					if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
					{
						wxBusyCursor wait;
						wxGxDatasetContainer* pCont = wxDynamicCast(pGxObject, wxGxDatasetContainer);
						if (!pCont->HasChildren())
							continue;
						const wxGxObjectList lObj = pCont->GetChildren();
						for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
						{
							IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(*it);
							if (NULL != pGxDSet)
							{
								paDatasets.push_back(pGxDSet);
							}
						}
					}
					else if (NULL != pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
					{
						paDatasets.push_back(dynamic_cast<IGxDataset*>(pGxObject));
					}
                }
            }

            //2. GxObject progress
            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
            if (paDatasets.size() == 1)
            {
                ExportSingleDatasetSelect(pWnd, paDatasets[0]);
            }
            else if (paDatasets.size() > 1)
            {
                ExportMultipleDatasetsSelect(pWnd, paDatasets);
            }
		}
		break;
	case 2:
		if (NULL != pSel && NULL != pCat)
		{
            //1. fill the IGxDataset* array
            wxVector<IGxDataset*> paDatasets;
            for (size_t i = 0; i < pSel->GetCount(); ++i)
            {
                wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
                if (NULL != pGxObject)
                {
                    if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
                    {
                        wxBusyCursor wait;
                        wxGxDatasetContainer* pCont = wxDynamicCast(pGxObject, wxGxDatasetContainer);
                        if (!pCont->HasChildren())
                            continue;
                        const wxGxObjectList lObj = pCont->GetChildren();
                        for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
                        {
                            IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(*it);
                            if (NULL != pGxDSet)
                            {
                                paDatasets.push_back(pGxDSet);
                            }
                        }
                    }
                    else if (NULL != pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
                    {
                        paDatasets.push_back(dynamic_cast<IGxDataset*>(pGxObject));
                    }
                }
            }

            //2. GxObject progress
            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
            if (paDatasets.size() == 1)
            {
                ExportSingleDatasetSelectWithParams(pWnd, paDatasets[0]);
            }
            else if (paDatasets.size() > 1)
            {
                ExportMultipleDatasetsSelectWithParams(pWnd, paDatasets);
            }
        }
		break;
	default:
		return;
	}
}

bool wxGISGeoprocessingCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
	m_pGxApp = dynamic_cast<wxGxApplicationBase*>(pApp);
	return true;
}

wxString wxGISGeoprocessingCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show/Hide Toolbox panel"));
		case 1:
			return wxString(_("Export item(s)"));
		case 2:
			return wxString(_("Export item with parameters"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISGeoprocessingCmd::GetCount(void)
{
	return 3;
}


