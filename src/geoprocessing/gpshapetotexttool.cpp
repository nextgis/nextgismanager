/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  write shape cordinates to text file
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

#include "wxgis/geoprocessing/gpshapetotexttool.h"
//#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/core/config.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPShapeToTextTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPShapeToTextTool, wxGISGPTool)

wxGISGPShapeToTextTool::wxGISGPShapeToTextTool(void) : wxGISGPTool()
{
	m_bEmptyCoordsMask = false;
}

wxGISGPShapeToTextTool::~wxGISGPShapeToTextTool(void)
{
}

const wxString wxGISGPShapeToTextTool::GetDisplayName(void)
{
    return wxString(_("Write shapes coordinates to text file"));
}

const wxString wxGISGPShapeToTextTool::GetName(void)
{
    return wxString(wxT("shape_to_text"));
}

const wxString wxGISGPShapeToTextTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters wxGISGPShapeToTextTool::GetParameterInfo(void)
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
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

		//mask
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("mask"));
        pParam2->SetDisplayName(_("Coordinates format mask to write to text file"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTStringChoiceEditable);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPStringDomain* pDomain2 = new wxGISGPStringDomain();

		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
		{
			wxXmlNode* pToolsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
			if(pToolsNode)
			{
				wxXmlNode* pChild = pToolsNode->GetChildren();
				while(pChild)
				{
					wxString sMask = pChild->GetAttribute(wxT("text"), wxEmptyString);
					if(!sMask.IsEmpty())
					{
						m_bEmptyCoordsMask = false;
						m_asCoordsMask.Add(sMask);
					}
					pChild = pChild->GetNext();
				}
			}
		}

		if(m_asCoordsMask.GetCount() == 0)
		{
			m_bEmptyCoordsMask = true;
			m_asCoordsMask.Add(wxT("dd-mm-ss.ss [W][ ]dd-mm-ss.ss [W]"));
#ifdef __WXMSW___
            m_asCoordsMask.Add(wxT("dd°mm'ss.ss\" [W][ ]dd°mm'ss.ss\" [W]"));
#endif
            m_asCoordsMask.Add(wxT("dd-mm-ss.ss[ ]dd-mm-ss.ss"));
            m_asCoordsMask.Add(wxT("ddmmss[ ]ddmmss"));
            m_asCoordsMask.Add(wxT("d.ddd[tab]d.ddd"));
            m_asCoordsMask.Add(wxT("d.ddd[ ]d.ddd"));
		}

        for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
			pDomain2->AddString(m_asCoordsMask[i], m_asCoordsMask[i]);

        pParam2->SetDomain(pDomain2);

        pParam2->SetValue(m_asCoordsMask[0]);

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

		//swap x y
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("swap_xy"));
        pParam3->SetDisplayName(_("Swap XY coordinates"));
        pParam3->SetParameterType(enumGISGPParameterTypeOptional);
        pParam3->SetDataType(enumGISGPParamDTBool);
        pParam3->SetDirection(enumGISGPParameterDirectionInput);
        pParam3->SetValue(false);

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));

		//dst path
        wxGISGPParameter* pParam4 = new wxGISGPParameter();
        pParam4->SetName(wxT("dst_path"));
        pParam4->SetDisplayName(_("Destination text file"));
        pParam4->SetParameterType(enumGISGPParameterTypeRequired);
        pParam4->SetDataType(enumGISGPParamDTPath);
        pParam4->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain4 = new wxGISGPGxObjectDomain();
		pDomain4->AddFilter(new wxGxTextFilter(wxString(_("Text file")), wxString(wxT(".txt"))));
        pParam4->SetDomain(pDomain4);

        m_paParam.Add(static_cast<IGPParameter*>(pParam4));

    }
    return m_paParam;
}

bool wxGISGPShapeToTextTool::Validate(void)
{
	//store in config if not exist
	if(m_bEmptyCoordsMask)
	{
		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
		{
			wxXmlNode* pToolsNode = oConfig.GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
			if(pToolsNode)
				oConfig.DeleteNodeChildren(pToolsNode);
			else
				pToolsNode = oConfig.CreateConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/coordinate_mask")));
			if(pToolsNode)
			{
				for(size_t i = 0; i < m_asCoordsMask.GetCount(); ++i)
				{
					wxXmlNode *pMskNode = new wxXmlNode(pToolsNode, wxXML_ELEMENT_NODE, wxString(wxT("mask")));
					pMskNode->AddAttribute(wxT("text"),  m_asCoordsMask[i]);
				}
			}
		}
		m_bEmptyCoordsMask = false;
	}

    if(!m_paParam[3]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_paParam[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_shape_to_text"));
			Name.SetExt(wxT("txt"));
            m_paParam[3]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[3]->SetAltered(true);//??
        }
    }

	//check domain list in m_paParam[1] and if changed store in config
    wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_paParam[1]->GetDomain());
    if(poGPStringDomain)
	{
		wxArrayString asNewCoordsMask;
		for(size_t i = 0; i < poGPStringDomain->GetCount(); ++i)
			asNewCoordsMask.Add(poGPStringDomain->GetName(i));
		if(m_asCoordsMask != asNewCoordsMask)
		{
			m_asCoordsMask = asNewCoordsMask;
			m_bEmptyCoordsMask = true;
		}
	}
	//create message using 55.25 & 35.35 coord look's like
	bool bSwap = m_paParam[2]->GetValue();
	wxString sMask = m_paParam[1]->GetValue();
	m_CFormat.Create(sMask, bSwap);
	if(m_CFormat.IsOk())
		m_paParam[1]->SetMessage(wxGISEnumGPMessageOk, m_CFormat.Format(55.25, 35.35));
    return true;
}

bool wxGISGPShapeToTextTool::Execute(ITrackCancel* pTrackCancel)
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
        return false;
    }

	bool bSwap = m_paParam[2]->GetValue();
	wxString sMask = m_paParam[1]->GetValue();
	m_CFormat.Create(sMask, bSwap);
	if(!m_CFormat.IsOk())
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Invalid format mask"), -1, enumGISMessageErr);
        return false;
    }

    wxString sDstPath = m_paParam[3]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

	IGxObject* pGxDstObject = GetParentGxObjectFromPath(sDstPath, pGxObjectContainer, pTrackCancel);
    if(!pGxDstObject)
        return false;

    CPLString szPath = pGxDstObject->GetInternalName();
    wxFileName sDstFileName(sDstPath);
    wxString sName = sDstFileName.GetFullName();

    bool bRes = GeometryVerticesToTextFile(pSrcDataSet, CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), NULL), m_CFormat, NULL, pTrackCancel);

    if(pGxDstObject)
		pGxDstObject->Refresh();

    return bRes;

}

