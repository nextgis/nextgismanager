/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Geoprocessing Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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

#include "wxgis/geoprocessingui/geoprocessingcmd.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/geoprocessing/gpdomain.h"

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
			if (paDatasets[0]->GetType() == enumGISRasterDataset)
			{
				if (paDatasets.size() == 1)
				{
					ExportSingleRasterDataset(paDatasets[0]);
				}
				else if (paDatasets.size() > 1)
				{
					ExportMultipleRasterDatasets(paDatasets);
				}
			}
			else if (paDatasets[0]->GetType() == enumGISFeatureDataset)
			{
				if (paDatasets.size() == 1)
				{
					ExportSingleVectorDataset(paDatasets[0]);
				}
				else if (paDatasets.size() > 1)
				{
					ExportMultipleVectorDatasets(paDatasets);
				}
			}
			else if (paDatasets[0]->GetType() == enumGISTableDataset)
			{
				if (paDatasets.size() == 1)
				{
					ExportSingleTableDataset(paDatasets[0]);
				}
				else if (paDatasets.size() > 1)
				{
					ExportMultipleTableDatasets(paDatasets);
				}
            }
		}
		break;
	case 2:
		if (NULL != pSel && NULL != pCat)
		{
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

void wxGISGeoprocessingCmd::ExportSingleVectorDataset(IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pGxDataset, wxT("The input pointer (IGxDataset*) is NULL"));

	wxGxObject* pGxSrcObj = dynamic_cast<wxGxObject*>(pGxDataset);
	wxString sName = pGxSrcObj->GetName();
	sName = ClearExt(sName);

	wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
	wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select output"));
	dlg.SetName(sName);
	dlg.SetAllowMultiSelect(false);
	dlg.SetAllFilters(false);


	wxGISEnumVectorDatasetType eSubType = (wxGISEnumVectorDatasetType)pGxDataset->GetSubType();
	bool bDefaultSet = false;
    wxGISEnumVectorDatasetType eDefaulSubType = enumVecUnknown;

    for (size_t i = enumVecUnknown + 1; i < emumVecMAX; ++i)
    {
        wxGISEnumVectorDatasetType eCurrentSubType = (wxGISEnumVectorDatasetType)i;
        if (eCurrentSubType != eSubType && IsFileDataset(enumGISFeatureDataset, eCurrentSubType))
        {
            if (bDefaultSet)
            {
                dlg.AddFilter(new wxGxFeatureDatasetFilter(eCurrentSubType), false);
            }
            else
            {
                dlg.AddFilter(new wxGxFeatureDatasetFilter(eCurrentSubType), true);
                bDefaultSet = true;
                eDefaulSubType = eCurrentSubType;
            }
        }
    }

    if (eSubType != emumVecPostGIS)
    {
        dlg.AddFilter(new wxGxFeatureDatasetFilter(emumVecPostGIS), false);
    }

	dlg.SetButtonCaption(_("Export"));
	dlg.SetOverwritePrompt(true);

	wxGxObject* pGxParentObj = pGxSrcObj->GetParent();
	wxString sStartLoc;

	if (pGxParentObj)
	{
		while (NULL != pGxParentObj)
		{
			wxGxObjectContainer* pGxCont = wxDynamicCast(pGxParentObj, wxGxObjectContainer);
            if (NULL != pGxCont && pGxCont->CanCreate(enumGISFeatureDataset, eDefaulSubType))
			{
				break;
			}
			else
			{
				pGxParentObj = pGxParentObj->GetParent();
			}
		}
		if (pGxParentObj)
		{
			sStartLoc = pGxParentObj->GetFullName();
		}
	}

    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_vector_ds/path"), sStartLoc);
    }

	if (!sStartLoc.IsEmpty())
		dlg.SetStartingLocation(sStartLoc);
	if (dlg.ShowModalSave() == wxID_OK)
	{

		wxGxObjectFilter* pFilter = dlg.GetCurrentFilter();
		if (NULL == pFilter)
		{
            wxMessageBox(_("Unexpected error"), _("Error"), wxCENTRE | wxOK | wxICON_ERROR, pWnd);
			wxLogError(_("Null wxGxObjectFilter returned"));
			return;
		}

        CPLString sPath = dlg.GetPath();
		wxString sCatalogPath = dlg.GetLocation()->GetFullName();
        wxFileName oFName(dlg.GetName());
        wxString sName = oFName.GetName();

        if (oConfig.IsOk())
        {
            oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_vector_ds/path"), sCatalogPath);
        }


        wxGISProgressDlg ProgressDlg(_("Exporting..."), _("Begin operation..."), 100, pWnd, wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);
        ProgressDlg.SetAddPercentToMessage(true);

        wxGISDataset* pDataset = pGxDataset->GetDataset(false, &ProgressDlg);
        wxGISFeatureDataset* pFeatureDataset = wxDynamicCast(pDataset, wxGISFeatureDataset);

        if (NULL == pFeatureDataset)
		{
			wxMessageBox(_("The dataset is empty"), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
			wxLogError(_("wxGISFeatureDataset pointer is null returned"));
			return;
		}

		//create progress dialog
        if (!pFeatureDataset->IsOpened())
		{
            if (!pFeatureDataset->Open(0, 0, false, &ProgressDlg))
			{
                wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                wxLogError(ProgressDlg.GetLastMessage());
                wsDELETE(pFeatureDataset);
				return;
			}
		}

        if (!ExportFormat(pFeatureDataset, sPath, sName, pFilter, wxGISNullSpatialFilter, NULL, NULL, static_cast<ITrackCancel*>(&ProgressDlg)))
		{
            wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
            wxLogError(ProgressDlg.GetLastMessage());
		}

        wsDELETE(pFeatureDataset);
	}
}

void wxGISGeoprocessingCmd::ExportSingleRasterDataset(IGxDataset* const pGxDataset)
{

}

void wxGISGeoprocessingCmd::ExportSingleTableDataset(IGxDataset* const pGxDataset)
{

}

void wxGISGeoprocessingCmd::ExportMultipleVectorDatasets(wxVector<IGxDataset*> &paDatasets)
{
/*	wxGxContainerDialog dlg(pWnd, pExtCat, wxID_ANY, _("Select output"));
	dlg.SetAllFilters(false);
	dlg.ShowExportFormats(true);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKML), true);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKMZ), true);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile), false);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab), false);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif), false);
	dlg.AddFilter(new wxGxFeatureFileFilter(enumVecDXF), false);

	dlg.AddShowFilter(new wxGxFolderFilter());
	dlg.ShowCreateButton(true);
	if (dlg.ShowModal() == wxID_OK)
	{
		IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
		if (!pFilter)
		{
			wxMessageBox(wxString(_("Null IGxObjectFilter returned")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
			wxLogError(_("Null IGxObjectFilter returned"));
			return;
		}

		CPLString sPath = dlg.GetInternalPath();
		wxString sCatalogPath = dlg.GetPath();

		//create progress dialog
		wxString sTitle = wxString::Format(_("%s %d objects (files)"), _("Process"), DatasetArray.size());
		wxWindow* pParentWnd = dynamic_cast<wxWindow*>(m_pApp);
		wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), 100, pParentWnd, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

		for (size_t i = 0; i < DatasetArray.size(); ++i)
		{
			wxString sMessage = wxString::Format(_("%s %d object (file) from %d"), _("Process"), i + 1, DatasetArray.size());
			ProgressDlg.SetTitle(sMessage);
			ProgressDlg.PutMessage(sMessage);
			if (!ProgressDlg.Continue())
				break;

			IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[i]);
			wxString sName = pGxSrcObj->GetName();

			wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[i]->GetSubType();
			if (emumVecPostGIS != nSubType)
				sName = ClearExt(sName);
			//if types is same skip exporting
			if (nSubType == pFilter->GetSubType())
				continue;

			wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(DatasetArray[i]->GetDataset(false, &ProgressDlg));
			if (!pDSet)
			{
				ProgressDlg.PutMessage(wxString::Format(_("The %d dataset is empty"), i + 1));
				continue;
			}
			if (!pDSet->IsOpened())
			if (!pDSet->Open(0, 0, false, &ProgressDlg))
				return;


			if (!ExportFormat(pDSet, sPath, sName, pFilter, NULL, &ProgressDlg))
			{
				wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxICON_ERROR | wxOK | wxCENTRE, pWnd);
				//while(!ProgressDlg.WasCancelled())
				//	wxMilliSleep(100);
				return;
			}
			else
			{
				//ProgressDlg.PutMessage(wxString::Format(_("Export successful (%s.%s)!"), sName.c_str(), pFilter->GetExt().c_str()));
				ProgressDlg.SetValue(ProgressDlg.GetValue() + 1);
			}
		}
		ProgressDlg.Destroy();

	}
}
*/
}

void wxGISGeoprocessingCmd::ExportMultipleRasterDatasets(wxVector<IGxDataset*> &paDatasets)
{

}

void wxGISGeoprocessingCmd::ExportMultipleTableDatasets(wxVector<IGxDataset*> &paDatasets)
{

}
