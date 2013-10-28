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
/*
#include "wxgis/geoprocessingui/geoprocessingcmd.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/framework/progressdlg.h"

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

//	0	Export
//  1   Show/hide toolbox pane
//  2   ?

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
			if(!m_IconGPMenu.IsOk())
				m_IconGPMenu = wxIcon(export_xpm);
			return m_IconGPMenu;
		case 1:
			if(!m_IconToolview.IsOk())
				m_IconToolview = wxIcon(toolview_xpm);
			return m_IconToolview;
		default:
			return wxNullIcon;
	}
}

wxString wxGISGeoprocessingCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Export"));
		case 1:
			return wxString(_("Show/Hide &Toolbox panel"));
		default:
		    return wxEmptyString;
	}
}

wxString wxGISGeoprocessingCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Geoprocessing"));
		case 1:
			return wxString(_("View"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISGeoprocessingCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 1:
            return m_pApp->IsApplicationWindowShown(m_pToolboxView);
		case 0:
		default:
	        return false;
	}
	return false;
}

bool wxGISGeoprocessingCmd::GetEnabled(void)
{
	if(!m_pToolboxView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxAxToolboxView* pwxAxToolboxView = dynamic_cast<wxAxToolboxView*>(pWinArr->at(i));
				if(pwxAxToolboxView)
				{
					m_pToolboxView = pwxAxToolboxView;
					break;
				}
			}
		}
	}

    switch(m_subtype)
	{
		case 0://Export
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxDataset* pDSet = dynamic_cast<IGxDataset*>(pGxObject.get());
                    if(pDSet)
                        return true;
                }
			}
			return false;
        }
		case 1://Show/Hide Toolbox panel
            return m_pToolboxView != NULL;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISGeoprocessingCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 1://Show/hide toolbox panel
            return enumGISCommandCheck;
		case 0://Export
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISGeoprocessingCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Export item to another format"));
		case 1:
			return wxString(_("Show/Hide Toolbox panel"));
		default:
			return wxEmptyString;
	}
}

void wxGISGeoprocessingCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			{
                //1. get the GxObject's list
                std::vector<IGxDataset*> DatasetArray;
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
                    wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
                    for(size_t i = 0; i < pSel->GetCount(); ++i)
                    {
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                        IGxDataset* pGxDSet = dynamic_cast<IGxDataset*>(pGxObject.get());
                        IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxDSet);
                        if(pObjectContainer)
                        {
							wxBusyCursor wait;
                            if(!pObjectContainer->HasChildren())
                                continue;
                            GxObjectArray* pArr = pObjectContainer->GetChildren();
                            if(pArr != NULL)
                            {
                                for(size_t i = 0; i < pArr->size(); ++i)
                                {
                                    pGxDSet = dynamic_cast<IGxDataset*>(pArr->operator[](i));
                                    if(pGxDSet)
                                        DatasetArray.push_back(pGxDSet);
                                }
                            }
                        }
                        else if(pGxDSet)
                            DatasetArray.push_back(pGxDSet);
                    }
                }

                wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                //2. GxObject progress
                if(DatasetArray.size() == 1)
                {
                    IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[0]);
                    wxString sName = pGxSrcObj->GetName();
                    sName = ClearExt(sName);

                    IGxObject* pGxParentObj = pGxSrcObj->GetParent();
                    wxString sStartLoc;
                    if(pGxParentObj)
                        sStartLoc = pGxParentObj->GetFullName();

                    IGxCatalog *pExtCat = NULL;
                    if(pGxApp)
                        pExtCat = pGxApp->GetCatalog();
                    wxGxObjectDialog dlg(pWnd, pExtCat, wxID_ANY, _("Select output"));
                    IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                    dlg.SetName(sName);
				    dlg.SetAllowMultiSelect(false);
				    dlg.SetAllFilters(false);

                    wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[0]->GetSubType();
                    if(nSubType != enumVecKML)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKML), true);
                    if(nSubType != enumVecKMZ)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKMZ), true);
                    if(nSubType != enumVecESRIShapefile)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile), false);
                    if(nSubType != enumVecMapinfoTab)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab), false);
                    if(nSubType != enumVecMapinfoMif)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif), false);
                    if(nSubType != enumVecDXF)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecDXF), false);

				    dlg.SetButtonCaption(_("Export"));
                    dlg.SetOverwritePrompt(true);
                    if(!sStartLoc.IsEmpty())
                        dlg.SetStartingLocation(sStartLoc);
                    if(dlg.ShowModalSave() == wxID_OK)
                    {

                        IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
                        if(!pFilter)
                        {
                            wxLogError(_("Null IGxObjectFilter returned"));
                            return;
                        }

                        CPLString sPath = dlg.GetInternalPath();
                        wxString sCatalogPath = dlg.GetPath();
                        wxString sName = dlg.GetName();

						wxString sTitle = wxString::Format(_("%s %d objects (files)"), _("Process"), DatasetArray.size());
						wxWindow* pParentWnd = dynamic_cast<wxWindow*>(m_pApp);
						wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), 100, pParentWnd, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);						
                        wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(DatasetArray[0]->GetDataset(false, &ProgressDlg));
                        if(!pDSet)
                        {
                            wxMessageBox(wxString(_("The dataset is empty")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("Null wxGISDataset returned"));
                            return;
                        }

						//create progress dialog
						if(!pDSet->IsOpened())
						{
							if(!pDSet->Open(0, 0, false, &ProgressDlg))
							{
                                wxMessageBox(ProgressDlg.GetLastMessage(), _("Error"), wxICON_ERROR | wxOK | wxCENTRE, pWnd);
								//ProgressDlg.PutMessage(_("Open dataset failed!"));
								//while(!ProgressDlg.WasCancelled())
								//	wxMilliSleep(100);
								return;
							}
						}
						//ITrackCancel TrackCancel;
      //                  IStatusBar* pStatusBar = m_pApp->GetStatusBar();                            
      //                  TrackCancel.SetProgressor(pStatusBar->GetProgressor());

                        if( !ExportFormat(pDSet, sPath, sName, pFilter, NULL, &ProgressDlg) )//&TrackCancel
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
						ProgressDlg.Destroy();

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sCatalogPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                    }
                }
                //3. if not single GxObject progress in special dialog
                else
                {
                    IGxCatalog *pExtCat = NULL;
                    if(pGxApp)
                        pExtCat = pGxApp->GetCatalog();
                    wxGxContainerDialog dlg(pWnd, pExtCat, wxID_ANY, _("Select output"));
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
                    if(dlg.ShowModal() == wxID_OK)
                    {
                        IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
                        if(!pFilter)
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

                        for(size_t i = 0; i < DatasetArray.size(); ++i)
                        {
							wxString sMessage = wxString::Format(_("%s %d object (file) from %d"), _("Process"), i + 1, DatasetArray.size());
							ProgressDlg.SetTitle(sMessage);
							ProgressDlg.PutMessage(sMessage);
							if(!ProgressDlg.Continue())
								break;

                            IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[i]);
                            wxString sName = pGxSrcObj->GetName();

                            wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[i]->GetSubType();
                            if(emumVecPostGIS != nSubType)
                                sName = ClearExt(sName);
                            //if types is same skip exporting
                            if(nSubType == pFilter->GetSubType())
                                continue;

                            wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(DatasetArray[i]->GetDataset(false, &ProgressDlg));
                            if(!pDSet)
                            {
                                ProgressDlg.PutMessage(wxString::Format(_("The %d dataset is empty"), i + 1));
                                continue;
                            }
							if(!pDSet->IsOpened())
								if(!pDSet->Open(0, 0, false, &ProgressDlg))
									return;


                            if( !ExportFormat(pDSet, sPath, sName, pFilter, NULL, &ProgressDlg) )
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

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sCatalogPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                    }
                }
                return;
			}
			break;
		case 1:
            m_pApp->ShowApplicationWindow(m_pToolboxView, !m_pApp->IsApplicationWindowShown(m_pToolboxView));
		default:
			return;
	}
}

bool wxGISGeoprocessingCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISGeoprocessingCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Export item"));
		case 1:
			return wxString(_("Show/Hide Toolbox panel"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISGeoprocessingCmd::GetCount(void)
{
	return 2;
}
*/