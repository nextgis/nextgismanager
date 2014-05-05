/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  base geoprocessing tool.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geoprocessing/gptool.h"

#include "wx/tokenzr.h"

int CPL_STDCALL ExecToolProgress( double dfComplete, const char *pszMessage, void *pData)
{
    bool bCancel = false;
    ITrackCancel* pTrackCancel = (ITrackCancel*)pData;

    if(pTrackCancel)
    {
        if( pszMessage )
        {
            wxString soMsg(pszMessage, wxConvUTF8);
            if(!soMsg.IsEmpty())
                pTrackCancel->PutMessage( soMsg, -1, enumGISMessageNorm );
        }
        IProgressor* pRogress = pTrackCancel->GetProgressor();
        if( pRogress )
            pRogress->SetValue((int) (dfComplete * 100));
        bCancel = !pTrackCancel->Continue();
    }
    return !bCancel;
}


wxGISGPTool::wxGISGPTool(void)
{
	m_pCatalog = NULL;
}

void wxGISGPTool::Copy(IGPTool* const pTool)
{
    if(pTool)
    {
        m_pCatalog = pTool->GetCatalog();
        SetFromString(pTool->GetAsString());
    }
    else
        m_pCatalog = NULL;
}

wxGISGPTool::~wxGISGPTool(void)
{
	for(size_t i = 0; i < m_paParam.size(); ++i)
        wxDELETE(m_paParam[i]);
}

void wxGISGPTool::SetCatalog(IGxCatalog* pCatalog)
{
    m_pCatalog = pCatalog;
}

IGxCatalog* const wxGISGPTool::GetCatalog(void)
{
    return m_pCatalog;
}

const wxString wxGISGPTool::GetAsString(void)
{
    wxString sOutParam;
    for(size_t i = 0; i < m_paParam.size(); ++i)
    {
        IGPParameter* pParam = m_paParam[i];
        if(pParam)
        {
            sOutParam += pParam->GetAsString();
            sOutParam += GPTOOLSEPARATOR;
        }
    }
    return sOutParam;
}

bool wxGISGPTool::SetFromString(const wxString& sParams)
{
    GetParameterInfo();
	wxStringTokenizer tkz(sParams, wxString(GPTOOLSEPARATOR), wxTOKEN_RET_EMPTY );
    size_t counter(0);
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
        if(counter >= m_paParam.size())
           return false;
        IGPParameter* pParam = m_paParam[counter];
        if(pParam)
        {
            if(!pParam->SetFromString(token))
                return false;
            else
                pParam->SetAltered(true);
        }
        counter++;
	}
    return true;
}
