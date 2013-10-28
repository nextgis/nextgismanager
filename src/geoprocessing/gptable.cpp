/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main table analysis functions.
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
#include "wxgis/geoprocessing/gptable.h"

wxGISTableSPtr CreateTable(CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, wxGISEnumTableDatasetType nType, char ** papszDataSourceOptions, char ** papszLayerOptions)
{
    CPLErrorReset();
    poFields->Reference();
	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( sDriver.mb_str() );
    if(poDriver == NULL)
    {
        wxString sErr = wxString::Format(_("The driver '%s' is not available! OGR error: "), sDriver.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        return wxGISTableSPtr();
    }

    CPLString sFullPath = CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    OGRDataSource *poDS = poDriver->CreateDataSource(sFullPath, papszDataSourceOptions );
    if(poDS == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output file '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISTableSPtr();
    }

    sName.Replace(wxT("."), wxT("_"));
    sName.Replace(wxT(" "), wxT("_"));
    sName.Replace(wxT("&"), wxT("_"));
	if(wxIsdigit(sName.GetChar(0)))
		sName.Prepend(_("Layer_"));
    sName.Truncate(27);
    CPLString szName = CPLString(sName.mb_str());

	OGRLayer *poLayerDest = poDS->CreateLayer(szName, NULL, wkbUnknown, papszLayerOptions );
    if(poLayerDest == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISTableSPtr();
    }

    for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
    {
        OGRFieldDefn *pField = poFields->GetFieldDefn(i);
        if( poLayerDest->CreateField( pField ) != OGRERR_NONE )
        {
            wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
            CPLString sFullErr(sErr.mb_str());
            sFullErr += CPLGetLastErrorMsg();
            CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
            return wxGISTableSPtr();
        }
    }


    poFields->Release();

    wxGISTableSPtr pDataSet = boost::make_shared<wxGISTable>(sFullPath, nType, poLayerDest, poDS);
    return pDataSet;
}

bool MeanValByColumn(wxGISTableSPtr pDSet, CPLString sPath, wxString sName, std::vector<FIELDMERGEDATA> &FieldMergeData, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Calculate mean values for columns. Source dataset %s. Destination dataset %s"), pDSet->GetName().c_str(), sName.c_str()), -1, enumGISMessageTitle);

	OGRFeatureDefn* const pDef = pDSet->GetDefinition();
    OGRFeatureDefn *pNewDef = new OGRFeatureDefn("mean_vals");

	int nBaseField = 0;
	for(size_t i = 0; i < FieldMergeData.size(); ++i)
	{
		OGRFieldDefn* pFldDefn = pDef->GetFieldDefn( FieldMergeData[i].nFieldPos );
		pNewDef->AddFieldDefn( new OGRFieldDefn(pFldDefn) );
		if(FieldMergeData[i].nOp == enumGISFMOMergeBase)
			nBaseField = FieldMergeData[i].nFieldPos;
	}

	const char *apszOptions[3] = { "CREATE_CSVT=YES", "SEPARATOR=SEMICOLON", NULL};
    wxGISTableSPtr pNewDSet = CreateTable(sPath, sName, sExt, sDriver, pNewDef, (wxGISEnumTableDatasetType)nNewSubType, NULL, (char **)apszOptions);
    if(!pNewDSet)
    {
        wxString sErr(_("Error creating new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), -1, enumGISMessageErr);
        return false;
    }

	std::map<CPLString, LPVALARRAY> mapped_data;

 	OGRFeatureSPtr poFeature;
	pDSet->Reset();
    while((poFeature = pDSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
			return false;

		CPLString szBase = poFeature->GetFieldAsString(nBaseField);
		bool bCreated = false;
		if(mapped_data[szBase] == NULL)
		{
			mapped_data[szBase] = new VALARRAY;
			bCreated = true;
		}
		for(size_t i = 0; i < FieldMergeData.size(); ++i)
		{
			OGRFieldDefn* pFldDefn = pDef->GetFieldDefn( FieldMergeData[i].nFieldPos );
			wxVariant Val;
			if(bCreated)
			{
				wxVariant DefVal;
				switch(pFldDefn->GetType())
				{
				case OFTInteger:
					DefVal = wxVariant(0);
					break;
				case OFTReal:
					DefVal = wxVariant(0.0);
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
					DefVal = wxVariant(wxT(""));
					break;
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
				default:
					DefVal = wxVariant();
					break;
				}
				VAL in_v = {DefVal, 0, pFldDefn->GetType()};
				mapped_data[szBase]->push_back(in_v);
			}
			switch(pFldDefn->GetType())
			{
			case OFTInteger:
				Val = wxVariant(poFeature->GetFieldAsInteger(FieldMergeData[i].nFieldPos));
				break;
			case OFTReal:
				Val = wxVariant(poFeature->GetFieldAsDouble(FieldMergeData[i].nFieldPos));
				break;
			case OFTString:
			case OFTWideString:
			case OFTDate:			//TODO: FIX IT
			case OFTTime:			//TODO: FIX IT
			case OFTDateTime:		//TODO: FIX IT
				Val = wxVariant(wxString(poFeature->GetFieldAsString(FieldMergeData[i].nFieldPos), wxConvLocal));
				break;
			case OFTIntegerList:
			case OFTRealList:
			case OFTStringList:
			case OFTWideStringList:
			case OFTBinary:
				break;
			}
			switch(FieldMergeData[i].nOp)
			{
			case enumGISFMOMin:
				switch(pFldDefn->GetType())
				{
				case OFTInteger:
					if(Val.GetInteger() != 0 && (mapped_data[szBase]->operator [](i).sum.GetInteger() == 0 || mapped_data[szBase]->operator [](i).sum.GetInteger() > Val.GetInteger()))
						mapped_data[szBase]->operator [](i).sum = Val;
					break;
				case OFTReal:
					if(Val.GetDouble() != 0 && (mapped_data[szBase]->operator [](i).sum.GetDouble() == 0 || mapped_data[szBase]->operator [](i).sum.GetDouble() > Val.GetDouble()))
						mapped_data[szBase]->operator [](i).sum = Val;
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
					break;
				}
				break;
			case enumGISFMOMax:
				switch(pFldDefn->GetType())
				{
				case OFTInteger:
					if(Val.GetInteger() != 0 && (mapped_data[szBase]->operator [](i).sum.GetInteger() == 0 || mapped_data[szBase]->operator [](i).sum.GetInteger() < Val.GetInteger()))
						mapped_data[szBase]->operator [](i).sum = Val;
					break;
				case OFTReal:
					if(Val.GetDouble() != 0 && (mapped_data[szBase]->operator [](i).sum.GetDouble() == 0 || mapped_data[szBase]->operator [](i).sum.GetDouble() < Val.GetDouble()))
						mapped_data[szBase]->operator [](i).sum = Val;
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
					break;
				}
				break;
			case enumGISFMOMean:
				switch(pFldDefn->GetType())
				{
				case OFTInteger:
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.GetInteger() + Val.GetInteger());
					break;
				case OFTReal:
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.GetDouble() + Val.GetDouble());
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.MakeString() + wxT(" ") + Val.MakeString());
					break;
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
					break;
				}
				mapped_data[szBase]->operator [](i).count++;
				break;
			case enumGISFMOSum:
				switch(pFldDefn->GetType())
				{
				case OFTInteger:
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.GetInteger() + Val.GetInteger());
					break;
				case OFTReal:
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.GetDouble() + Val.GetDouble());
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
					mapped_data[szBase]->operator [](i).sum = (mapped_data[szBase]->operator [](i).sum.MakeString() + wxT(" ") + Val.MakeString());
					break;
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
					break;
				}
				break;
			case enumGISFMOMergeBase:
				mapped_data[szBase]->operator [](i).sum = Val;
				break;
			}
		}
		//double dT = poFeature->GetFieldAsDouble(1);
		//double dTMin = poFeature->GetFieldAsDouble(13);
		//double dTMax = poFeature->GetFieldAsDouble(14);
		//double dRRR = poFeature->GetFieldAsDouble(22);
		//CPLString sDate = poFeature->GetFieldAsString(0);
		//wxDateTime dt;
		//dt.ParseFormat(wxString(sDate, wxConvLocal), wxT("%d.%m.%Y"));// %H:%M
		//if(!dt.IsValid())
		//    continue;

		//maped_data[dt].mean_temp.sum += dT;
		//maped_data[dt].mean_temp.count++;
		//if(dTMin != 0 && (maped_data[dt].min_temp == 0 || maped_data[dt].min_temp > dTMin))
		//	maped_data[dt].min_temp = dTMin;
		//if(dTMax != 0 && (maped_data[dt].max_temp == 0 || maped_data[dt].max_temp < dTMin))
		//	maped_data[dt].max_temp = dTMax;
		//if(dRRR != 0 && (maped_data[dt].max_rrr == 0 || maped_data[dt].max_rrr < dRRR))
		//	maped_data[dt].max_rrr = dRRR;


	}

	IProgressor* pProgress(NULL);
    if(pTrackCancel)
		pProgress = pTrackCancel->GetProgressor();
	if(pProgress)
		pProgress->SetRange(mapped_data.size());

	int nCounter(0);
	for(std::map<CPLString, LPVALARRAY>::iterator it = mapped_data.begin(); it != mapped_data.end(); ++it)
	{
		if(pProgress)
			pProgress->SetValue(nCounter);

		OGRFeatureSPtr poFeature = pNewDSet->CreateFeature();
		for(size_t i = 0; i < it->second->size(); ++i)
		{
			wxVariant Val =  it->second->operator [](i).sum;
			switch(FieldMergeData[i].nOp)
			{
			case enumGISFMOMean:
				switch(it->second->operator [](i).nType)
				{
				case OFTInteger:
					Val = (Val.GetInteger() / it->second->operator [](i).count);
					break;
				case OFTReal:
					Val = (Val.GetDouble() / it->second->operator [](i).count);
					break;
				case OFTString:
				case OFTWideString:
				case OFTDate:			//TODO: FIX IT
				case OFTTime:			//TODO: FIX IT
				case OFTDateTime:		//TODO: FIX IT
				case OFTIntegerList:
				case OFTRealList:
				case OFTStringList:
				case OFTWideStringList:
				case OFTBinary:
					break;
				}
				break;
			case enumGISFMOSum:
			case enumGISFMOMin:
			case enumGISFMOMax:
			default:
				break;
			}

			switch(it->second->operator [](i).nType)
			{
			case OFTInteger:
				poFeature->SetField(i, (int)Val.GetInteger());
				break;
			case OFTReal:
				poFeature->SetField(i,Val.GetDouble());
				break;
			case OFTString:
			case OFTWideString:
			case OFTDate:			//TODO: FIX IT
			case OFTTime:			//TODO: FIX IT
			case OFTDateTime:		//TODO: FIX IT
				poFeature->SetField(i,Val.MakeString().mb_str());
				break;
			case OFTIntegerList:
			case OFTRealList:
			case OFTStringList:
			case OFTWideStringList:
			case OFTBinary:
				break;
			}
		//poFeature->SetField(0, it->first.GetYear(), it->first.GetMonth() + 1, it->first.GetDay());
		//poFeature->SetField(1, double(it->second.mean_temp.sum) / it->second.mean_temp.count);
		//poFeature->SetField(2, it->second.max_temp);
		//poFeature->SetField(3, it->second.min_temp);
		//poFeature->SetField(4, it->second.max_rrr);
		}
		OGRErr eErr = pNewDSet->StoreFeature(poFeature);

		nCounter++;

		wxDELETE(it->second);
	}

	return true;
}
