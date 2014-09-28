/******************************************************************************
 * Project:  wxGIS
 * Purpose:  global functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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

#include "wxgis/core/app.h"

IMPLEMENT_ABSTRACT_CLASS(IApplication, wxObject)

static IApplication* g_pApplication(NULL);

extern WXDLLIMPEXP_GIS_CORE IApplication* const GetApplication()
{
	return g_pApplication;
}

extern WXDLLIMPEXP_GIS_CORE void SetApplication(IApplication* pApp)
{
    if(g_pApplication != NULL)
        wxDELETE(g_pApplication);
	g_pApplication = pApp;
}


void wxGISLogError(const wxString& sAppErr, const wxString &sLibError, const wxString &sLibErrorPrepend, ITrackCancel* const pTrackCancel)
{
	if(NULL == pTrackCancel)
	{
		wxLogError(sAppErr + wxT(" -- ") + sLibError);	
		return;
	}
		
	wxString sErrMsg = sAppErr;
	if(!sLibError.IsEmpty())
	{
		if(!sLibErrorPrepend.IsEmpty())
		{
			sErrMsg.Append(wxT("\n") + sLibErrorPrepend);
		}
		else
		{
			sErrMsg.Append(wxT("\n"));
		}
		sErrMsg.Append(sLibError);
	}	
	
	wxLogError(sErrMsg);
	pTrackCancel->PutMessage(sErrMsg, wxNOT_FOUND, enumGISMessageErr);
}

void wxGISLogMessage(const wxString& sAppMsg, ITrackCancel* const pTrackCancel)
{
	if(NULL == pTrackCancel)
	{
		wxLogMessage(sAppMsg);	
		return;
	}
		
	wxLogMessage(sAppMsg);	
	pTrackCancel->PutMessage(sAppMsg, wxNOT_FOUND, enumGISMessageInfo);
}

