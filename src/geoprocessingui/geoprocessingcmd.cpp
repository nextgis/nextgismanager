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
		case enumGISGeoprocessingCmdShowHideToolboxPane:
			if (!m_IconToolView.IsOk())
				m_IconToolView = wxIcon(toolview_xpm);
			return m_IconToolView;
		case enumGISGeoprocessingCmdExport:
		case enumGISGeoprocessingCmdExportWithParameters:
        case enumGISGeoprocessingCmdExportAttrbutes:
			if(!m_IconGPMenu.IsOk())
				m_IconGPMenu = wxIcon(export_xpm);
			return m_IconGPMenu;
		case enumGISGeoprocessingCmdImport:
			if(!m_IconImport.IsOk())
			{
				wxBitmap oExportOrigin(export_xpm);
				wxImage oImport = oExportOrigin.ConvertToImage();
				oImport = oImport.Mirror(true);

				m_IconImport.CopyFromBitmap(wxBitmap(oImport));
			}
			return m_IconImport;
		default:
			return wxNullIcon;
	}
}

wxString wxGISGeoprocessingCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case enumGISGeoprocessingCmdShowHideToolboxPane:
			return wxString(_("Show/Hide &Toolbox panel"));
		case enumGISGeoprocessingCmdExport:
			return wxString(_("&Export"));
		case enumGISGeoprocessingCmdExportWithParameters:
			return wxString(_("E&xport with parameters"));
		case enumGISGeoprocessingCmdExportAttrbutes:
			return wxString(_("Export &attributes"));
		case enumGISGeoprocessingCmdImport:
			return wxString(_("&Import"));		
		default:
		    return wxEmptyString;
	}
}

wxString wxGISGeoprocessingCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISGeoprocessingCmdShowHideToolboxPane:
			return wxString(_("View"));
		case enumGISGeoprocessingCmdExport:
		case enumGISGeoprocessingCmdExportWithParameters:
        case enumGISGeoprocessingCmdExportAttrbutes:
		case enumGISGeoprocessingCmdImport:
			return wxString(_("Geoprocessing"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISGeoprocessingCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case enumGISGeoprocessingCmdShowHideToolboxPane:
            return m_pApp->IsApplicationWindowShown(m_pToolboxView);
		case enumGISGeoprocessingCmdExport:
		case enumGISGeoprocessingCmdExportWithParameters:
		case enumGISGeoprocessingCmdExportAttrbutes:
		case enumGISGeoprocessingCmdImport:
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
		case enumGISGeoprocessingCmdShowHideToolboxPane:
            return NULL != m_pToolboxView;
		case enumGISGeoprocessingCmdExport:
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
		case enumGISGeoprocessingCmdExportWithParameters:
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
        case enumGISGeoprocessingCmdExportAttrbutes:
            if (NULL != pSel && NULL != pCat)
			{
				for (size_t i = 0; i < pSel->GetCount(); ++i)
				{
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					wxGxDataset* pDSet = wxDynamicCast(pGxObject, wxGxDataset);
					if (NULL != pDSet && pDSet->GetType() == enumGISFeatureDataset)
					{
						return true;
					}
				}
			}
            return false;
		case enumGISGeoprocessingCmdImport:
            if (NULL != pSel && NULL != pCat)
			{
				for (size_t i = 0; i < pSel->GetCount(); ++i)
				{
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					IGxObjectEditUI* pObjEditUI = dynamic_cast<IGxObjectEditUI*>(pGxObject);
					if (NULL != pObjEditUI && pObjEditUI->CanImport())
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
		case enumGISGeoprocessingCmdShowHideToolboxPane:
            return enumGISCommandCheck;
		case enumGISGeoprocessingCmdExport:
		case enumGISGeoprocessingCmdExportWithParameters:
        case enumGISGeoprocessingCmdExportAttrbutes:
		case enumGISGeoprocessingCmdImport:
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISGeoprocessingCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case enumGISGeoprocessingCmdShowHideToolboxPane:
			return wxString(_("Show/Hide Toolbox panel"));
		case enumGISGeoprocessingCmdExport:
			return wxString(_("Export selected item(s) to another format"));
		case enumGISGeoprocessingCmdExportWithParameters:
			return wxString(_("Export selected item to another format"));
		case enumGISGeoprocessingCmdExportAttrbutes:
			return wxString(_("Export selected item attributes"));
		case enumGISGeoprocessingCmdImport:	
			return wxString(_("Import into selected item"));
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
	case enumGISGeoprocessingCmdShowHideToolboxPane:
		m_pApp->ShowApplicationWindow(m_pToolboxView, !m_pApp->IsApplicationWindowShown(m_pToolboxView));
        break;
	case enumGISGeoprocessingCmdExport:
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
						if (!pCont->HasChildren(true))
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
					else if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
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
	case enumGISGeoprocessingCmdExportWithParameters:
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
                        if (!pCont->HasChildren(true))
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
                    else if (pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
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
    case enumGISGeoprocessingCmdExportAttrbutes:
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
                        if (!pCont->HasChildren(true))
                            continue;
                        const wxGxObjectList lObj = pCont->GetChildren();
                        for (wxGxObjectList::const_iterator it = lObj.begin(); it != lObj.end(); ++it)
                        {
                            IGxDataset *pGxDSet = dynamic_cast<IGxDataset*>(*it);
                            if (NULL != pGxDSet && pGxDSet->GetType() == enumGISFeatureDataset)
                            {
                                paDatasets.push_back(pGxDSet);
                            }
                        }
                    }
                    else
                    {
                        wxGxDataset* pDSet = wxDynamicCast(pGxObject, wxGxDataset);
                        if (NULL != pDSet && pDSet->GetType() == enumGISFeatureDataset)
                        {
                            paDatasets.push_back(dynamic_cast<IGxDataset*>(pGxObject));
                        }
                    }
                }
            }

            //2. GxObject progress
            wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
            if (paDatasets.size() == 1)
            {
                ExportSingleDatasetAttributes(pWnd, paDatasets[0]);
            }
            else if (paDatasets.size() > 1)
            {
                ExportMultipleDatasetsAttributes(pWnd, paDatasets);
            }
        }
        break;
		case enumGISGeoprocessingCmdImport:	
			if (NULL != pSel && NULL != pCat)
			{
				for (size_t i = 0; i < pSel->GetCount(); ++i)
				{
					wxGxObject* pGxObject = pCat->GetRegisterObject(pSel->GetSelectedObjectId(i));
					IGxObjectEditUI* pObjEditUI = dynamic_cast<IGxObjectEditUI*>(pGxObject);
					if (NULL != pObjEditUI && pObjEditUI->CanImport())
					{
						wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
						pObjEditUI->Import(pWnd);
					}
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
		case enumGISGeoprocessingCmdShowHideToolboxPane:
			return wxString(_("Show/Hide Toolbox panel"));
		case enumGISGeoprocessingCmdExport:
			return wxString(_("Export item(s)"));
		case enumGISGeoprocessingCmdExportWithParameters:
			return wxString(_("Export item with parameters"));
		case enumGISGeoprocessingCmdExportAttrbutes:
			return wxString(_("Export item's attributes"));
		case enumGISGeoprocessingCmdImport:
			return wxString(_("Import into the item"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISGeoprocessingCmd::GetCount(void)
{
	return enumGISGeoprocessingCmdMax;
}


