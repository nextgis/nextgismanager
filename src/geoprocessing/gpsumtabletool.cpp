/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  calc mean column data by other colum with equil values
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/geoprocessing/gpsumtabletool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gptable.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"
#include "wxgis/datasource/table.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPMeanByColumnTool, wxGISGPTool)

wxGISGPMeanByColumnTool::wxGISGPMeanByColumnTool(void) : wxGISGPTool()
{
}

wxGISGPMeanByColumnTool::~wxGISGPMeanByColumnTool(void)
{
}

const wxString wxGISGPMeanByColumnTool::GetDisplayName(void)
{
    return wxString(_("Calculate mean column data"));
}

const wxString wxGISGPMeanByColumnTool::GetName(void)
{
    return wxString(wxT("mean_by_col"));
}

const wxString wxGISGPMeanByColumnTool::GetCategory(void)
{
    return wxString(_("Statistics Tools/Table"));
}

GPParameters wxGISGPMeanByColumnTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source table"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISTableDataset));
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

		//field map
		wxGISGPMultiParameter* pParam2 = new wxGISGPMultiParameter();
        pParam2->SetName(wxT("src_field_map"));
        pParam2->SetDisplayName(_("Source fields and operations"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);
		pParam2->AddColumn(_("Field name"));
		pParam2->AddColumn(_("Operation"));

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination table"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
		pDomain3->AddFilter(new wxGxTableFilter(enumTableCSV));
        pParam3->SetDomain(pDomain3);

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));

    }
    return m_paParam;
}

bool wxGISGPMeanByColumnTool::Validate(void)
{
    wxString sPath = m_paParam[0]->GetValue();
	if(sPath.CmpNoCase(m_sInputPath) != 0)
    {
        if(m_paParam[0]->GetIsValid())
        {
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxString(wxT("_")) + GetName());
            m_paParam[2]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[2]->SetAltered(true);//??

			//fill fields list
			IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
			if(!pGxObjectContainer)
			{
				m_paParam[0]->SetIsValid(false);
				m_paParam[0]->SetMessage(wxGISEnumGPMessageError, _("The GxCatalog is undefined type (not inherited from IGxObjectContainer)"));
				return false;
			}
			IGxObject* pGxObject = pGxObjectContainer->SearchChild(sPath);
			if(!pGxObject)
			{
				m_paParam[0]->SetIsValid(false);
				m_paParam[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("There is no GxObject for path %s"), sPath.c_str()));
				return false;
			}
			IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
			if(!pGxDataset)
			{
				m_paParam[0]->SetIsValid(false);
				m_paParam[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("The input path is not GxDataset (Path %s)"), sPath.c_str()));
				return false;
			}
           	wxGISTableSPtr pSrcTable = boost::dynamic_pointer_cast<wxGISTable>(pGxDataset->GetDataset());
			if(!pSrcTable)
			{
				m_paParam[0]->SetIsValid(false);
				m_paParam[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("The input path is not wxGISTable (Path %s)"), sPath.c_str()));
				return false;
			}
			if(!pSrcTable->IsOpened())
			{
				if(!pSrcTable->Open())
				{
					m_paParam[0]->SetIsValid(false);
					m_paParam[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("Failed open input path: %s"), sPath.c_str()));
					return false;
				}
			}
		    
			OGRFeatureDefn *pDef = pSrcTable->GetDefinition();
			if(!pDef)
			{
				m_paParam[0]->SetIsValid(false);
				m_paParam[0]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("Failed get table structure (Path %s)"), sPath.c_str()));
				return false;
			}
			wxGISGPMultiParameter* pParam2 = dynamic_cast<wxGISGPMultiParameter*>(m_paParam[1]);
			pParam2->Clear();
			for(size_t i = 0; i < pDef->GetFieldCount(); ++i)
			{
				OGRFieldDefn* pFieldDefn = pDef->GetFieldDefn(i);
				wxString sFieldName(pFieldDefn->GetNameRef(), wxConvLocal);

				wxGISGPParameter* pParamP1 = new wxGISGPParameter();
				pParamP1->SetName(wxT("field_name"));
				pParamP1->SetDisplayName(sFieldName);
				pParamP1->SetParameterType(enumGISGPParameterTypeRequired);
				pParamP1->SetDataType(enumGISGPParamDTString);
				pParamP1->SetDirection(enumGISGPParameterDirectionInput);

				wxVariant val(sFieldName);
				pParamP1->SetValue(val);

				pParam2->AddParameter(0, i, pParamP1);

				wxGISGPParameter* pParamP2 = new wxGISGPParameter();
				pParamP2->SetName(wxT("field_name"));
				pParamP2->SetDisplayName(sFieldName);
				pParamP2->SetParameterType(enumGISGPParameterTypeRequired);
				pParamP2->SetDataType(enumGISGPParamDTInteger);
				pParamP2->SetDirection(enumGISGPParameterDirectionInput);

				wxGISGPValueDomain* pDomain2 = new wxGISGPValueDomain();
				pDomain2->AddValue(enumGISFMONone, wxString(_("Exclude field")));
				pDomain2->AddValue(enumGISFMOMergeBase, wxString(_("Base field")));
				pDomain2->AddValue(enumGISFMOMean, wxString(_("Mean value")));
				pDomain2->AddValue(enumGISFMOMin, wxString(_("Minimum value")));
				pDomain2->AddValue(enumGISFMOMax, wxString(_("Maximum value")));
				pDomain2->AddValue(enumGISFMOSum, wxString(_("Sum of values")));
				pParamP2->SetDomain(pDomain2);
				pParamP2->SetSelDomainValue(0);

				pParam2->AddParameter(1, i, pParamP2);
			}
			m_paParam[1]->SetHasBeenValidated(false);
			m_paParam[1]->SetAltered(false);
			m_sInputPath = sPath;
		}
    }
    if(m_paParam[1]->GetAltered())
    {
		//check column 1 have the only one base field value
		int nBaseFieldCount = 0;
		wxGISGPMultiParameter* pParam2 = dynamic_cast<wxGISGPMultiParameter*>(m_paParam[1]);
		for(size_t i = 0; i < pParam2->GetRowCount(); ++i)
		{
			IGPParameter* pCellParam = pParam2->GetParameter(1, i);
			if(pCellParam->GetValue().GetInteger() == enumGISFMOMergeBase)
				nBaseFieldCount++;
		}
		if(nBaseFieldCount != 1)
		{
            m_paParam[1]->SetIsValid(false);
			m_paParam[1]->SetMessage(wxGISEnumGPMessageError, wxString::Format(_("The base field count should be 1 (current count is %d)"), nBaseFieldCount));
            return false;
		}
		else
		{
            m_paParam[1]->SetIsValid(true);
			m_paParam[1]->SetMessage(wxGISEnumGPMessageOk);
		}
	}
    return true;
}

bool wxGISGPMeanByColumnTool::Execute(ITrackCancel* pTrackCancel)
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
    wxGISTableSPtr pSrcTable = boost::dynamic_pointer_cast<wxGISTable>(pGxDataset->GetDataset());
    if(!pSrcTable)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
	if(!pSrcTable->IsOpened())
		if(!pSrcTable->Open())
			return false;
    
    OGRFeatureDefn *pDef = pSrcTable->GetDefinition();
    if(!pDef)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error reading dataset definition"), -1, enumGISMessageErr);
        return false;
    }
    
    wxString sDstPath = m_paParam[2]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

	IGxObject* pGxDstObject = GetParentGxObjectFromPath(sDstPath, pGxObjectContainer, pTrackCancel);
    if(!pGxDstObject)
        return false;

    CPLString szPath = pGxDstObject->GetInternalName();
    wxFileName sDstFileName(sDstPath);
    wxString sName = sDstFileName.GetName();
    
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[2]->GetDomain());
	IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[2]->GetSelDomainValue());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }

	std::vector<FIELDMERGEDATA> FieldMergeData;
	wxGISGPMultiParameter* pParam2 = dynamic_cast<wxGISGPMultiParameter*>(m_paParam[1]);
	for(size_t i = 0; i < pParam2->GetRowCount(); ++i)
	{
		IGPParameter* pCellParam = pParam2->GetParameter(1, i);
		wxGISFieldMergeOperator nOp = (wxGISFieldMergeOperator)pCellParam->GetValue().GetInteger();
		if(nOp == enumGISFMONone)
			continue;
		IGPParameter* pCellParam1 = pParam2->GetParameter(0, i);		
		CPLString szFieldName(pCellParam1->GetValue().GetString().mb_str()); 
		int nPos = pDef->GetFieldIndex(szFieldName);
		FIELDMERGEDATA InputData = {nPos, nOp};
		FieldMergeData.push_back(InputData);
	}

        
	//field/operarator [sum,avrg,ignore,column] array
	//FIELDMERGEDATA InputData1 = {0, enumGISFMOMergeBase};
	//FieldMergeData.push_back(InputData1);
	//FIELDMERGEDATA InputData2 = {2, enumGISFMOMean};//T
	//FieldMergeData.push_back(InputData2);
	//FIELDMERGEDATA InputData3 = {3, enumGISFMOMean};//Po
	//FieldMergeData.push_back(InputData3);
	//FIELDMERGEDATA InputData4 = {14, enumGISFMOMin};//TMin
	//FieldMergeData.push_back(InputData4);
	//FIELDMERGEDATA InputData5 = {15, enumGISFMOMax};//TMax
	//FieldMergeData.push_back(InputData5);
	//FIELDMERGEDATA InputData6 = {23, enumGISFMOSum};//RRR
	//FieldMergeData.push_back(InputData6);
	//FIELDMERGEDATA InputData1 = {1, enumGISFMOMergeBase};
	//FieldMergeData.push_back(InputData1);
	//FIELDMERGEDATA InputData2 = {8, enumGISFMOMean};//T
	//FieldMergeData.push_back(InputData2);
	//FIELDMERGEDATA InputData3 = {14, enumGISFMOMean};//Po
	//FieldMergeData.push_back(InputData3);
	//FIELDMERGEDATA InputData4 = {15, enumGISFMOMin};//TMin
	//FieldMergeData.push_back(InputData4);
	//FIELDMERGEDATA InputData5 = {16, enumGISFMOMax};//TMax
	//FieldMergeData.push_back(InputData5);
	//FIELDMERGEDATA InputData6 = {17, enumGISFMOSum};//RRR
	//FieldMergeData.push_back(InputData6);

    bool bRes = MeanValByColumn(pSrcTable, szPath, sName, FieldMergeData, pFilter, NULL, pTrackCancel);

    if(pGxDstObject)
        pGxDstObject->Refresh();

    return bRes;
}