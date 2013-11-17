/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  geoproessing vector datasets.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalogui/processing.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/catalogui/gxobjdialog.h"

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
    else if (eType == enumGISTableDataset)
    {
        for (size_t i = enumTableUnknown + 1; i < emumTableMAX; ++i)
        {
            wxGISEnumTableDatasetType eCurrentSubType = (wxGISEnumTableDatasetType)i;
            if (eCurrentSubType != eSubType && IsFileDataset(enumGISTableDataset, eCurrentSubType))
            {
                if (bDefaultSet)
                {
                    dlg.AddFilter(new wxGxTableDatasetFilter(eCurrentSubType), false);
                }
                else
                {
                    dlg.AddFilter(new wxGxTableDatasetFilter(eCurrentSubType), true);
                    bDefaultSet = true;
                    eDefaulSubType = eCurrentSubType;
                }
            }
        }

        if (eSubType != enumTablePostgres)
        {
            dlg.AddFilter(new wxGxTableDatasetFilter(enumTablePostgres), false);
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
        else if (eType == enumGISTableDataset)
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
            else if (eType == enumGISTableDataset)
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
        else if (eType == enumGISTableDataset)
        {
            ExportSingleTableDataset(pWnd, sPath, sName, pFilter, pGxDataset);
        }
    }
}

void ExportSingleDatasetSelectWithParams(wxWindow* pWnd, IGxDataset* const pGxDataset)
{
    //TODO:
    ExportSingleDatasetSelect(pWnd, pGxDataset);
}

void ExportMultipleDatasetsSelect(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets)
{

}

void ExportMultipleDatasetsSelectWithParams(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets)
{
    //TODO:
    ExportMultipleDatasetsSelect(pWnd, paDatasets);
}

void ExportSingleVectorDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{
    wxCHECK_RET(pFilter && pGxDataset, wxT("The input pointer is NULL"));

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
    else if (ProgressDlg.GetWarningCount() > 0)
    {
        wxMessageDialog dlg(pWnd, _("During export there were several warnings."), _("Warning"), wxOK | wxCENTRE | wxICON_WARNING);
        wxString extMsg;
        const wxVector<MESSAGE>& msgs = ProgressDlg.GetWarnings();
        for (size_t i = 0; i < msgs.size(); ++i)
        {
            if (msgs[i].eType == enumGISMessageErr)
                extMsg += _("Error") + wxT(": ");
            else if (msgs[i].eType == enumGISMessageWarning)
                extMsg += _("Warning") + wxT(": ");
            extMsg += msgs[i].sMessage;
            extMsg += wxT("\n");
        }
        dlg.SetExtendedMessage(extMsg);
        dlg.ShowModal();
    }

    wsDELETE(pFeatureDataset);
}

void ExportSingleRasterDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{

}

void ExportSingleTableDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset)
{

}

void ExportMultipleVectorDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
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

void ExportMultipleRasterDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
{

}

void ExportMultipleTableDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets)
{

}

#endif // wxGIS_HAVE_GEOPROCESSING
