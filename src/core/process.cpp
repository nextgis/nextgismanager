/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  stream common classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Dmitry Baryshnikov
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

#include "wxgis/core/process.h"

#include <wx/txtstrm.h>

#define READ_LINE_DELAY 50
//------------------------------------------------------------------------------
// Class wxGISProcess
//------------------------------------------------------------------------------

wxGISProcess::wxGISProcess(IGISProcessParent* pParent) : wxProcess(wxPROCESS_REDIRECT), wxThreadHelper(wxTHREAD_DETACHED) //wxTHREAD_DETACHED//wxTHREAD_JOINABLE
{
    m_pParent = pParent;
    m_dtBeg = wxDateTime::Now();
    m_dtEstEnd = wxDateTime::Now();
    m_pid = 0;
    m_bKill = false;
}

wxGISProcess::~wxGISProcess(void)
{
}

bool wxGISProcess::Start()
{
	m_pid = Execute();
    if(m_pid == 0)
        return false;

	m_nState = enumGISTaskWork;
	m_dtBeg = wxDateTime::Now();
    m_dfDone = 0;
	//create and start read thread stdin
	if(IsInputOpened())
	{
        return CreateAndRunReadThread();
	}

    //start err thread ?
    return true; //or false?
}

void wxGISProcess::OnTerminate(int pid, int status)
{
    DestroyReadThread();
    if(m_nState == enumGISTaskPaused)//process paused
    {
        m_dfDone = 0;
        if(m_pParent)
            m_pParent->OnFinish(this, false);
        return;
    }

    if(m_pParent && m_nState == enumGISTaskWork)
    {
        m_dfDone = 100.0;
        if(m_pParent)
            m_pParent->OnFinish(this, status != 0);
    }

	m_nState = status == 0 ? enumGISTaskDone : enumGISTaskError;
    m_dtEstEnd = wxDateTime::Now();
}

void wxGISProcess::Stop(void)
{
    if (m_nState == enumGISTaskDone || m_nState == enumGISTaskError)
        return;

    wxCriticalSectionLocker lock(m_ExitLock);

	if(m_nState == enumGISTaskWork)
	{
        if (m_pid != 0)
        {
		    wxKillError eErr = Kill(m_pid, wxSIGKILL);// wxSIGINT wxSIGTERM

            if (eErr == wxKILL_OK)
            {
                m_pid = 0;

                DestroyReadThread();
	           //and detach
		        Detach();
            }
        }

	}
	m_nState = enumGISTaskPaused;
    m_dtEstEnd = wxDateTime::Now();
}

void wxGISProcess::UpdatePercent(const wxString &sPercentData)
{
	m_dfDone = wxAtof(sPercentData);
	wxTimeSpan Elapsed = wxDateTime::Now() - m_dtBeg;//time left

    double nPercentR = 100.0 - m_dfDone;
	if(nPercentR >= 0.0 && m_dfDone > 0.0)
	{
		long dMSec = double(Elapsed.GetMilliseconds().ToDouble() * nPercentR) / m_dfDone;
		wxTimeSpan Remains = wxTimeSpan(0,0,0,dMSec);
		m_dtEstEnd = wxDateTime::Now() + Remains;
	}
}

void wxGISProcess::ProcessInput(wxString &sInputData)
{
    wxCriticalSectionLocker lock(m_ExitLock);
	if(m_nState != enumGISTaskWork)
        return;
    sInputData = sInputData.Trim(true).Trim(false);


	wxString sRest;
	if( sInputData.StartsWith(wxT("DONE: "), &sRest) )
	{
        UpdatePercent(sRest.Trim(true).Trim(false).Truncate(sRest.Len() - 1));
		return;
	}
	if( sInputData.StartsWith(wxT("INFO: "), &sRest) )
	{
        AddInfo(enumGISMessageNorm, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("ERR: "), &sRest) )
	{
        AddInfo(enumGISMessageErr, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("WARN: "), &sRest) )
	{
        AddInfo(enumGISMessageWarning, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("SND: "), &sRest) )
	{
        AddInfo(enumGISMessageSend, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("RCV: "), &sRest) )
	{
        AddInfo(enumGISMessageReceive, sRest);
		return;
	}

    AddInfo(enumGISMessageUnk, sInputData);
}

wxThread::ExitCode wxGISProcess::Entry()
{
    // IMPORTANT:
    // this function gets executed in the secondary thread context!

    // here we do our long task, periodically calling TestDestroy():

    //wxTextInputStream (wxInputStream &stream, const wxString &sep=" \t", const wxMBConv &conv=wxConvAuto())
    wxInputStream &InStream = *GetInputStream();
	wxTextInputStream InputStr(InStream, wxT(" \t"), *wxConvCurrent);
    while (!GetThread()->TestDestroy())
    {
        if (m_bKill || InStream.Eof())
            break;
        if(InStream.IsOk() && InStream.CanRead())
        {
		    wxString line = InputStr.ReadLine();
		    ProcessInput(line);
        }
		wxThread::Sleep(READ_LINE_DELAY);
    }

    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

bool wxGISProcess::CreateAndRunReadThread(void)
{
    if (!GetThread())
    {
        if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)//wxTHREAD_JOINABLE//wxTHREAD_DETACHED
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    m_bKill = false;

    if (!GetThread()->IsRunning())
    {
        if (GetThread()->Run() != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not run the thread!"));
            return false;
        }
    }
    return true;
}

void wxGISProcess::DestroyReadThread(void)
{
    if (GetThread() && GetThread()->IsRunning())
    {
        m_bKill = true;
        GetThread()->Delete();//Wait();//Kill();//
    }
}

