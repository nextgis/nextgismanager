/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  export geoprocessing tools.
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

#include "wxgis/geoprocessing/gpprojecttool.h"
//#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"

#include "wx/filename.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPProjectVectorTool, wxGISGPTool)

wxGISGPProjectVectorTool::wxGISGPProjectVectorTool(void) : wxGISGPTool()
{
}

wxGISGPProjectVectorTool::~wxGISGPProjectVectorTool(void)
{
}

const wxString wxGISGPProjectVectorTool::GetDisplayName(void)
{
    return wxString(_("Project vector file (single)"));
}

const wxString wxGISGPProjectVectorTool::GetName(void)
{
    return wxString(wxT("vproj_single"));
}

const wxString wxGISGPProjectVectorTool::GetCategory(void)
{
    return wxString(_("Projections and Transformations/Vector"));
}

GPParameters wxGISGPProjectVectorTool::GetParameterInfo(void)
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

        //proj
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("proj"));
        pParam2->SetDisplayName(_("Spatial reference"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTSpatRef);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination feature class"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
//        pDomain3->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pDomain3->AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile));
        pDomain3->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab));
        pDomain3->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif));
        pParam3->SetDomain(pDomain3);

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));
    }
    return m_paParam;
}

bool wxGISGPProjectVectorTool::Validate(void)
{
    if(!m_paParam[2]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //generate temp name
            wxFileName Name(m_paParam[0]->GetValue());
            Name.SetName(Name.GetName() + wxT("_reproject"));
            m_paParam[2]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[2]->SetAltered(true);//??
        }
    }

    //check if input & output types is same!
    //if(m_paParam[0]->GetIsValid())
    //{
    //    if(!m_paParam[2]->GetHasBeenValidated())
    //    {
    //        wxFileName Name1(m_paParam[0]->GetValue());
    //        wxFileName Name2(m_paParam[2]->GetValue());
    //        if(Name1.GetExt() != Name2.GetExt())
    //        {
    //            m_paParam[2]->SetIsValid(false);
    //            m_paParam[2]->SetMessage(wxGISEnumGPMessageError, _("Cannot project to the different format"));
    //            return false;
    //        }
    //    }
    //}
    return true;
}

bool wxGISGPProjectVectorTool::Execute(ITrackCancel* pTrackCancel)
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

    //get source
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

    //get spatial reference
    wxString sWKT = m_paParam[1]->GetValue().MakeString();
    if(sWKT.IsEmpty())
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unsupported Spatial Reference"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szWKT(sWKT.mb_str());
    OGRSpatialReference NewSpaRef;
    
    const char* szpWKT = szWKT.c_str();
    if(NewSpaRef.importFromWkt((char**)&szpWKT) != OGRERR_NONE)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unsupported Spatial Reference"), -1, enumGISMessageErr);
        return false;
    }

    //get destination
    wxString sDstPath = m_paParam[2]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

	IGxObject* pGxDstObject = GetParentGxObjectFromPath(sDstPath, pGxObjectContainer, pTrackCancel);
    if(!pGxDstObject)
        return false;

	CPLString szDestPath = pGxDstObject->GetInternalName();
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


    bool bRes = Project(pSrcDataSet, szDestPath, sName, pFilter, &NewSpaRef, pTrackCancel);

    //CPLString szPath = pGxDstObject->GetInternalName();
    //wxString sName = sDstFileName.GetName();
    //
    //wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
    //IGxObjectFilter* pFilter = pDomain->GetFilter(pDomain->GetSelFilter());
    //if(!pFilter)
    //{
    //    //add messages to pTrackCancel
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
    //    return false;
    //}
    //    
    //bool bHasErrors = ExportFormat(pSrcDataSet, szPath, sName, pFilter, NULL, pTrackCancel);

    if(pGxDstObject)
        pGxDstObject->Refresh();

    return bRes;
}
