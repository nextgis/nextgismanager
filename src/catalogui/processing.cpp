/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  geoproessing vector datasets.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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

#include "wxgis/catalogui/processing.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxcontdialog.h"

#ifdef wxGIS_HAVE_GEOPROCESSING

#include "wxgis/geoprocessing/gpdomain.h"

void ExportSingleDatasetSelect(wxWindow* pWnd, IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pGxDataset, wxT("The input pointer (IGxDataset*) is NULL"));


    wxGxObject* pGxSrcObj = dynamic_cast<wxGxObject*>(pGxDataset);
    wxString sName = pGxSrcObj->GetName();
    sName = ClearExt(sName);

    wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select output"));
    dlg.SetName(sName);
    dlg.SetAllowMultiSelect(false);
    dlg.SetAllFilters(false);


    wxGISEnumDatasetType eType = pGxDataset->GetType();
    int eSubType = pGxDataset->GetSubType();
    bool bDefaultSet = false;
    int eDefaulSubType = 0;

    if (eType == enumGISFeatureDataset)
    {
        for (size_t i = enumVecUnknown + 1; i < enumVecMAX; ++i)
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

        if (eSubType != enumVecPostGIS)
        {
            dlg.AddFilter(new wxGxFeatureDatasetFilter(enumVecPostGIS), false);
        }
    }
    else if (eType == enumGISRasterDataset)
    {
        //for (size_t i = enumRasterUnknown + 1; i < enumRasterMAX; ++i)
        //{
        //    wxGISEnumRasterDatasetType eCurrentSubType = (wxGISEnumRasterDatasetType)i;
        //    if (eCurrentSubType != eSubType && IsFileDataset(enumGISRasterDataset, eCurrentSubType))
        //    {
        //        if (bDefaultSet)
        //        {
        //            dlg.AddFilter(new wxGxRasterDatasetFilter(eCurrentSubType), false);
        //        }
        //        else
        //        {
        //            dlg.AddFilter(new wxGxRasterDatasetFilter(eCurrentSubType), true);
        //            bDefaultSet = true;
        //            eDefaulSubType = eCurrentSubType;
        //        }
        //    }
        //}

        //if (eSubType != enumRasterPostGIS)
        //{
        //    dlg.AddFilter(new wxGxRasterDatasetFilter(enumRasterPostGIS), false);
        //}
    }
    else if (eType == enumGISTable)
    {
        for (size_t i = enumTableUnknown + 1; i < enumTableMAX; ++i)
        {
            wxGISEnumTableType eCurrentSubType = (wxGISEnumTableType)i;
            if (eCurrentSubType != eSubType && IsFileDataset(enumGISTable, eCurrentSubType))
            {
                if (bDefaultSet)
                {
                    dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), false);
                }
                else
                {
                    dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), true);
                    bDefaultSet = true;
                    eDefaulSubType = eCurrentSubType;
                }
            }
        }

        if (eSubType != enumTablePostgres)
        {
            dlg.AddFilter(new wxGxTableFilter(enumTablePostgres), false);
        }
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
        if (eType == enumGISFeatureDataset)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_vector_ds/path"), sStartLoc);
        }
        else if (eType == enumGISRasterDataset)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_raster_ds/path"), sStartLoc);
        }
        else if (eType == enumGISTable)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_table_ds/path"), sStartLoc);
        }
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
            if (eType == enumGISFeatureDataset)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_vector_ds/path"), sCatalogPath);
            }
            else if (eType == enumGISRasterDataset)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_raster_ds/path"), sCatalogPath);
            }
            else if (eType == enumGISTable)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_table_ds/path"), sCatalogPath);
            }
        }

        if (eType == enumGISFeatureDataset)
        {
            ExportSingleVectorDataset(pWnd, sPath, sName, pFilter, pGxDataset);
        }
        else if (eType == enumGISRasterDataset)
        {
            ExportSingleRasterDataset(pWnd, sPath, sName, pFilter, pGxDataset);
        }
        else if (eType == enumGISTable)
        {
            ExportSingleTable(pWnd, sPath, sName, pFilter, pGxDataset);
        }
    }
}

void ExportSingleDatasetSelectWithParams(wxWindow* pWnd, IGxDataset* const pGxDataset)
{
    //TODO:
    ExportSingleDatasetSelect(pWnd, pGxDataset);
}

void ExportSingleDatasetAttributes(wxWindow* pWnd, IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pGxDataset, wxT("The input pointer (IGxDataset*) is NULL"));

    wxGxObject* pGxSrcObj = dynamic_cast<wxGxObject*>(pGxDataset);
    wxString sName = pGxSrcObj->GetName();
    sName = ClearExt(sName);

    wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select output"));
    dlg.SetName(sName);
    dlg.SetAllowMultiSelect(false);
    dlg.SetAllFilters(false);

    bool bDefaultSet = false;
    int eDefaulSubType = 0;

    for (size_t i = enumTableUnknown + 1; i < enumTableMAX; ++i)
    {
        wxGISEnumTableType eCurrentSubType = (wxGISEnumTableType)i;
        if (IsFileDataset(enumGISTable, eCurrentSubType))
        {
            if (bDefaultSet)
            {
                dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), false);
            }
            else
            {
                dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), true);
                bDefaultSet = true;
                eDefaulSubType = eCurrentSubType;
            }
        }
    }
    dlg.AddFilter(new wxGxTableFilter(enumTablePostgres), false);

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
        sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_attr/path"), sStartLoc);
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
            oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/exp_attr/path"), sCatalogPath);
        }

        ExportSingleTable(pWnd, sPath, sName, pFilter, pGxDataset);
    }
}


void ExportMultipleDatasetsSelect(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets)
{
    wxCHECK_RET(paDatasets.size() > 1, wxT("The input dataset array is empty"));

    wxGxContainerDialog dlg(pWnd, wxID_ANY, _("Select output"));
    dlg.SetAllFilters(false);
    dlg.ShowExportFormats(true);

    wxGxObject* pGxSrcObj = dynamic_cast<wxGxObject*>(paDatasets[0]);
    wxGISEnumDatasetType eType = paDatasets[0]->GetType();
    int eSubType = paDatasets[0]->GetSubType();
    bool bDefaultSet = false;
    int eDefaulSubType = 0;

    if (eType == enumGISFeatureDataset)
    {
        for (size_t i = enumVecUnknown + 1; i < enumVecMAX; ++i)
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

        if (eSubType != enumVecPostGIS)
        {
            dlg.AddFilter(new wxGxFeatureDatasetFilter(enumVecPostGIS), false);
        }
    }
    else if (eType == enumGISRasterDataset)
    {
        //for (size_t i = enumRasterUnknown + 1; i < enumRasterMAX; ++i)
        //{
        //    wxGISEnumRasterDatasetType eCurrentSubType = (wxGISEnumRasterDatasetType)i;
        //    if (eCurrentSubType != eSubType && IsFileDataset(enumGISRasterDataset, eCurrentSubType))
        //    {
        //        if (bDefaultSet)
        //        {
        //            dlg.AddFilter(new wxGxRasterDatasetFilter(eCurrentSubType), false);
        //        }
        //        else
        //        {
        //            dlg.AddFilter(new wxGxRasterDatasetFilter(eCurrentSubType), true);
        //            bDefaultSet = true;
        //            eDefaulSubType = eCurrentSubType;
        //        }
        //    }
        //}

        //if (eSubType != enumRasterPostGIS)
        //{
        //    dlg.AddFilter(new wxGxRasterDatasetFilter(enumRasterPostGIS), false);
        //}
    }
    else if (eType == enumGISTable)
    {
        for (size_t i = enumTableUnknown + 1; i < enumTableMAX; ++i)
        {
            wxGISEnumTableType eCurrentSubType = (wxGISEnumTableType)i;
            if (eCurrentSubType != eSubType && IsFileDataset(enumGISTable, eCurrentSubType))
            {
                if (bDefaultSet)
                {
                    dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), false);
                }
                else
                {
                    dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), true);
                    bDefaultSet = true;
                    eDefaulSubType = eCurrentSubType;
                }
            }
        }

        if (eSubType != enumTablePostgres)
        {
            dlg.AddFilter(new wxGxTableFilter(enumTablePostgres), false);
        }
    }


    dlg.AddShowFilter(new wxGxFolderFilter());
    dlg.AddShowFilter(new wxGxRemoteDBSchemaFilter());
    dlg.ShowCreateButton(true);

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
        if (eType == enumGISFeatureDataset)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_vector_ds/path"), sStartLoc);
        }
        else if (eType == enumGISRasterDataset)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_raster_ds/path"), sStartLoc);
        }
        else if (eType == enumGISTable)
        {
            sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_table_ds/path"), sStartLoc);
        }
    }

    if (!sStartLoc.IsEmpty())
        dlg.SetStartingLocation(sStartLoc);

    if (dlg.ShowModal() == wxID_OK)
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

        if (oConfig.IsOk())
        {
            if (eType == enumGISFeatureDataset)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_vector_ds/path"), sCatalogPath);
            }
            else if (eType == enumGISRasterDataset)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_raster_ds/path"), sCatalogPath);
            }
            else if (eType == enumGISTable)
            {
                oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_table_ds/path"), sCatalogPath);
            }
        }

        //TODO: Now we create the copies (new names) instead of overwrite, but should show table with exist names and new names. If user set the same name - overwrite
        // |----------------|------------------|
        // |    dataset1    |  dataset1 (1)    |
        // |    dataset2    |  dataset2 (1)    |
        // |    dataset3    |  dataset3 (2)    |
        // |----------------|------------------|

        wxVector<EXPORTED_DATASET> paExportDatasets;
        for (size_t i = 0; i < paDatasets.size(); ++i)
        {
            wxGxObject* pGxSrcDatasetObj = dynamic_cast<wxGxObject*>(paDatasets[i]);
            if (NULL == pGxSrcDatasetObj)
            {
                continue;
            }
            wxString sNewName = CheckUniqName(sPath, pGxSrcDatasetObj->GetBaseName(), pFilter->GetExt());
            EXPORTED_DATASET ds = { sNewName, paDatasets[i] };
            paExportDatasets.push_back(ds);
        }

        if (eType == enumGISFeatureDataset)
        {
            ExportMultipleVectorDatasets(pWnd, sPath, pFilter, paExportDatasets);
        }
        else if (eType == enumGISRasterDataset)
        {
            ExportMultipleRasterDatasets(pWnd, sPath, pFilter, paExportDatasets);
        }
        else if (eType == enumGISTable)
        {
            ExportMultipleTable(pWnd, sPath, pFilter, paExportDatasets);
        }
    }
}

void ExportMultipleDatasetsSelectWithParams(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets)
{
    //TODO: Add set parameter dialog?
    ExportMultipleDatasetsSelect(pWnd, paDatasets);
}

void ExportMultipleDatasetsAttributes(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets)
{
    wxCHECK_RET(paDatasets.size() > 1, wxT("The input dataset array is empty"));

    wxGxContainerDialog dlg(pWnd, wxID_ANY, _("Select output"));
    dlg.SetAllFilters(false);
    dlg.ShowExportFormats(true);

    wxGxObject* pGxSrcObj = dynamic_cast<wxGxObject*>(paDatasets[0]);

    bool bDefaultSet = false;
    int eDefaulSubType = 0;

    for (size_t i = enumTableUnknown + 1; i < enumTableMAX; ++i)
    {
        wxGISEnumTableType eCurrentSubType = (wxGISEnumTableType)i;
        if (IsFileDataset(enumGISTable, eCurrentSubType))
        {
            if (bDefaultSet)
            {
                dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), false);
            }
            else
            {
                dlg.AddFilter(new wxGxTableFilter(eCurrentSubType), true);
                bDefaultSet = true;
                eDefaulSubType = eCurrentSubType;
            }
        }
    }

    dlg.AddFilter(new wxGxTableFilter(enumTablePostgres), false);

    dlg.AddShowFilter(new wxGxFolderFilter());
    dlg.AddShowFilter(new wxGxRemoteDBSchemaFilter());
    dlg.ShowCreateButton(true);

    wxGxObject* pGxParentObj = pGxSrcObj->GetParent();
    wxString sStartLoc;

    if (pGxParentObj)
    {
        while (NULL != pGxParentObj)
        {
            wxGxObjectContainer* pGxCont = wxDynamicCast(pGxParentObj, wxGxObjectContainer);
            if (NULL != pGxCont && pGxCont->CanCreate(enumGISTable, eDefaulSubType))
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
        sStartLoc = oConfig.Read(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_attr/path"), sStartLoc);
    }

    if (!sStartLoc.IsEmpty())
        dlg.SetStartingLocation(sStartLoc);

    if (dlg.ShowModal() == wxID_OK)
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

        if (oConfig.IsOk())
        {
            oConfig.Write(enumGISHKCU, dlg.GetAppName() + wxT("/lastpath/expm_attr/path"), sCatalogPath);
        }

        //TODO: Now we create the copies (new names) instead of overwrite, but should show table with exist names and new names. If user set the same name - overwrite
        // |----------------|------------------|
        // |    dataset1    |  dataset1 (1)    |
        // |    dataset2    |  dataset2 (1)    |
        // |    dataset3    |  dataset3 (2)    |
        // |----------------|------------------|

        wxVector<EXPORTED_DATASET> paExportDatasets;
        for (size_t i = 0; i < paDatasets.size(); ++i)
        {
            wxGxObject* pGxSrcDatasetObj = dynamic_cast<wxGxObject*>(paDatasets[i]);
            if (NULL == pGxSrcDatasetObj)
            {
                continue;
            }
            wxString sNewName = CheckUniqName(sPath, pGxSrcDatasetObj->GetBaseName(), pFilter->GetExt());
            EXPORTED_DATASET ds = { sNewName, paDatasets[i] };
            paExportDatasets.push_back(ds);
        }

        ExportMultipleTable(pWnd, sPath, pFilter, paExportDatasets);
    }
}


void ExportSingleVectorDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pFilter && pGxDataset, wxT("The input pointer is NULL"));

    wxGISProgressDlg ProgressDlg(_("Exporting..."), _("Begin operation..."), 100, pWnd);
    ProgressDlg.SetAddPercentToMessage(false);
    ProgressDlg.ShowProgress(true);

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
        if (!pFeatureDataset->Open(0, false, true, false, &ProgressDlg))
        {
            wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
            wxLogError(ProgressDlg.GetLastMessage());
            wsDELETE(pFeatureDataset);
            return;
        }
    }

    //set default options
    char** papszLayerOptions = NULL;
    if(pFilter->GetSubType() == enumVecCSV)
    {
        //TODO: set from config
        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "GEOMETRY", "AS_WKT");
        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "CREATE_CSVT", "YES");
        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "SEPARATOR", "SEMICOLON");
        papszLayerOptions = CSLAddNameValue(papszLayerOptions, "WRITE_BOM", "YES");
    }


    if (!ExportFormat(pFeatureDataset, sPath, sName, pFilter, wxGISNullSpatialFilter, NULL, papszLayerOptions, true, static_cast<ITrackCancel*>(&ProgressDlg)))
    {
        ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());
    }
    else if (ProgressDlg.GetWarningCount() > 0)
    {
        ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());
    }

    wsDELETE(pFeatureDataset);
}

void ExportSingleRasterDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{

}

void ExportSingleTable(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pFilter && pGxDataset, wxT("The input pointer is NULL"));

    wxGISProgressDlg ProgressDlg(_("Exporting..."), _("Begin operation..."), 100, pWnd);
    ProgressDlg.SetAddPercentToMessage(false);
    ProgressDlg.ShowProgress(true);

    wxGISDataset* pDataset = pGxDataset->GetDataset(false, &ProgressDlg);
    wxGISTable* pTable = wxDynamicCast(pDataset, wxGISTable);

    if (NULL == pTable)
    {
        wxMessageBox(_("The table is empty"), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
        wxLogError(_("wxGISFeatureDataset pointer is null returned"));
        return;
    }

    //create progress dialog
    if (!pTable->IsOpened())
    {
        if (!pTable->Open(0, false, true, false, &ProgressDlg))
        {
            wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
            wxLogError(ProgressDlg.GetLastMessage());
            wsDELETE(pTable);
            return;
        }
    }

    if (!ExportFormat(pTable, sPath, sName, pFilter, wxGISNullSpatialFilter, NULL, NULL, static_cast<ITrackCancel*>(&ProgressDlg)))
    {
        ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());
    }
    else if (ProgressDlg.GetWarningCount() > 0)
    {
        ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());
    }

    wsDELETE(pTable);
}

void ExportMultipleVectorDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
{
    wxCHECK_RET(pWnd && pFilter && paDatasets.size() > 1, wxT("The input pointer is NULL or datasets array is empty"));

    wxGISProgressDlg ProgressDlg(_("Exporting..."), _("Begin operation..."), 100, pWnd);
    ProgressDlg.SetAddPercentToMessage(false);
    ProgressDlg.ShowProgress(true);

    for (size_t i = 0; i < paDatasets.size(); ++i)
    {
        ProgressDlg.SetTitle(wxString::Format(_("Proceed %ld of %ld..."), i + 1, paDatasets.size()));
        wxGISDataset* pDataset = paDatasets[i].pDSet->GetDataset(false, &ProgressDlg);
        wxVector<wxGISFeatureDataset*> apFeatureDatasets;
        if (pDataset->GetSubsetsCount() == 0)
        {
            wxGISFeatureDataset* pFeatureDataset = wxDynamicCast(pDataset, wxGISFeatureDataset);
            if (NULL != pFeatureDataset)
            {
                pFeatureDataset->Reference();
                apFeatureDatasets.push_back(pFeatureDataset);
            }
        }
        else
        {
            for (size_t j = 0; j < pDataset->GetSubsetsCount(); ++j)
            {
                wxGISFeatureDataset* pFeatureDataset = wxDynamicCast(pDataset->GetSubset(j), wxGISFeatureDataset);
                if (NULL != pFeatureDataset)
                {
                    pFeatureDataset->Reference();
                    apFeatureDatasets.push_back(pFeatureDataset);
                }
            }
        }

        if (apFeatureDatasets.size() == 0)
        {
            wxMessageBox(_("The dataset is empty"), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
            wxLogError(_("wxGISFeatureDataset pointer is null returned"));
            return;
        }

        for (size_t j = 0; j < apFeatureDatasets.size(); ++j)
        {
            if(!ProgressDlg.Continue())
                break;
            if (!apFeatureDatasets[j]->IsOpened())
            {
                if (!apFeatureDatasets[j]->Open(0, true, true, false, &ProgressDlg))
                {
                    wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                    wxLogError(ProgressDlg.GetLastMessage());
                    wsDELETE(apFeatureDatasets[j]);
                    continue;
                }
            }

            if (!ExportFormat(apFeatureDatasets[j], sPath, paDatasets[i].sName, pFilter, wxGISNullSpatialFilter, NULL, NULL, true, static_cast<ITrackCancel*>(&ProgressDlg)))
            {
                wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                wxLogError(ProgressDlg.GetLastMessage());
                wsDELETE(apFeatureDatasets[j]);
                continue;
            }
            wsDELETE(apFeatureDatasets[j]);
        }
    }

    if (ProgressDlg.GetWarningCount() > 0 && ProgressDlg.Continue())
    {
        ShowMessageDialog(pWnd, ProgressDlg.GetWarnings());
    }
}

void ExportMultipleRasterDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
{

}

void ExportMultipleTable(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
{

}

void ShowMessageDialog(wxWindow* pWnd, const wxVector<MESSAGE>& msgs)
{
    wxMessageDialog dlg(pWnd, _("During export there were several warnings."), _("Warning"), wxOK | wxCENTRE | wxICON_WARNING);
    wxString extMsg;
    for (size_t i = 0; i < msgs.size(); ++i)
    {
        if (msgs[i].sMessage.IsEmpty())
            continue;
        if (msgs[i].eType == enumGISMessageErr)
            extMsg += _("Error") + wxT(": ");
        else if (msgs[i].eType == enumGISMessageWarning)
            extMsg += _("Warning") + wxT(": ");
        extMsg += msgs[i].sMessage;
        extMsg += wxT("\n\n");
    }
    dlg.SetExtendedMessage(extMsg);
    dlg.ShowModal();

    wxLogError(extMsg);
}

#endif // wxGIS_HAVE_GEOPROCESSING
