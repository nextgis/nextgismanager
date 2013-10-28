/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  export geoprocessing tools.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessing/gpexporttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPExportTool, wxGISGPTool)

wxGISGPExportTool::wxGISGPExportTool(void) : wxGISGPTool()
{
}

wxGISGPExportTool::~wxGISGPExportTool(void)
{
}

const wxString wxGISGPExportTool::GetDisplayName(void)
{
    return wxString(_("Export vector file (single)"));
}

const wxString wxGISGPExportTool::GetName(void)
{
    return wxString(wxT("vexport_single"));
}

const wxString wxGISGPExportTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters wxGISGPExportTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source feature class"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        ////SQL statement
        //wxGISGPParameter* pParam2 = new wxGISGPParameter();
        //pParam2->SetName(wxT("sql_statement"));
        //pParam2->SetDisplayName(_("SQL statement"));
        //pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        //pParam2->SetDataType(enumGISGPParamDTQuery);
        //pParam2->SetDirection(enumGISGPParameterDirectionInput);

        //m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination feature class"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
        AddAllVectorFilters(pDomain3);
        pParam3->SetDomain(pDomain3);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));

    }
    return m_paParam;
}

bool wxGISGPExportTool::Validate(void)
{
    //if(!m_paParam[2]->GetAltered())
    //{
    //    if(m_paParam[0]->GetIsValid())
    //    {
    //        //same name but different ext
    //        wxString sPath = m_paParam[0]->GetValue();
    //        m_paParam[2]->SetValue(wxVariant(sPath, wxT("path")));
    //        m_paParam[2]->SetAltered(true);//??
    //    }
    //}

    ////check if input & output types is same!
    //if(m_paParam[0]->GetIsValid())
    //{
    //    //TODO: Maybe IGxDataset in future?
    //    //IGxDataset* pDset1 = m_pCatalog->SearchChild()
    //    wxFileName Name1(m_paParam[0]->GetValue());
    //    wxFileName Name2(m_paParam[2]->GetValue());
    //    if(Name1.GetExt() == Name2.GetExt())
    //    {
    //        m_paParam[2]->SetIsValid(false);
    //        m_paParam[2]->SetMessage(wxGISEnumGPMessageError, _("Cannot export to the same format"));
    //        return false;
    //    }
    //}
    //return true;

	if(!m_paParam[1]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //same name but different ext
            wxString sPath = m_paParam[0]->GetValue();
            m_paParam[1]->SetValue(wxVariant(sPath, wxT("path")));
            m_paParam[1]->SetAltered(true);//??
        }
    }

    //check if input & output types is same!
    if(m_paParam[0]->GetIsValid())
    {
        //TODO: Maybe IGxDataset in future?
        //IGxDataset* pDset1 = m_pCatalog->SearchChild()
        wxFileName Name1(m_paParam[0]->GetValue());
        wxFileName Name2(m_paParam[1]->GetValue());
        if(Name1.GetExt() == Name2.GetExt())
        {
            m_paParam[1]->SetIsValid(false);
            m_paParam[1]->SetMessage(wxGISEnumGPMessageError, _("Cannot export to the same format"));
            return false;
        }
    }
    return true;

}

bool wxGISGPExportTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_paParam[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISFeatureDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(pGxDataset->GetDataset());
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
	if(!pSrcDataSet->IsOpened())
		if(!pSrcDataSet->Open())
			return false;
    
    OGRFeatureDefn *pDef = pSrcDataSet->GetDefinition();
    if(!pDef)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error reading dataset definition"), -1, enumGISMessageErr);
        //wsDELETE(pSrcDataSet);
        return false;
    }
    
//    wxString sDstPath = m_paParam[2]->GetValue();
    wxString sDstPath = m_paParam[1]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

	IGxObject* pGxDstObject = GetParentGxObjectFromPath(sDstPath, pGxObjectContainer, pTrackCancel);
    if(!pGxDstObject)
        return false;

	CPLString szPath = pGxDstObject->GetInternalName();
    wxFileName sDstFileName(sDstPath);
    wxString sName = sDstFileName.GetName();
    
    //wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[2]->GetDomain());
	//IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[2]->GetSelDomainValue());
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
	IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[1]->GetSelDomainValue());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }
        
    bool bRes = ExportFormat(pSrcDataSet, szPath, sName, pFilter, NULL, pTrackCancel);

    if(pGxDstObject)
        pGxDstObject->Refresh();

    return bRes;
}

