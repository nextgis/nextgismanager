/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  stream common classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012,2014 Dmitry Baryshnikov
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

#define READ_LINE_DELAY 550

//------------------------------------------------------------------------------
// Class wxGISProcess
//------------------------------------------------------------------------------

wxGISThreadHelper::wxGISThreadHelper(wxThreadKind kind) : wxThreadHelper(kind) //wxTHREAD_JOINABLE
{
    m_bKill = false;
}

wxGISThreadHelper::~wxGISThreadHelper()
{
    KillThread();
}

bool wxGISThreadHelper::CreateAndRunThread(void)
{
    wxCriticalSectionLocker locker(m_critSection);
    if (!m_thread)
    {
        if (CreateThread(m_kind) != wxTHREAD_NO_ERROR)//wxTHREAD_JOINABLE
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
		m_bKill = false;

        if (m_thread->Run() != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not run the thread!"));
            return false;
        }
    }
    return true;
}

bool wxGISThreadHelper::IsThreadRun() const
{
    wxCriticalSectionLocker locker((wxCriticalSection&)m_critSection); 
    return (m_thread && m_thread->IsRunning());
}

void wxGISThreadHelper::DestroyThreadAsync(void)
{
    if (m_thread)
    {
        m_bKill = true;
        if (m_kind == wxTHREAD_DETACHED)
		{
            m_thread->Delete();//Wait();//Kill();//
		}
        else if (m_kind == wxTHREAD_JOINABLE)
		{
            m_thread->Wait();//Kill();//
            wxCriticalSectionLocker locker(m_critSection);
            wxDELETE(m_thread);
		}
    }
}

void wxGISThreadHelper::DestroyThreadSync(int nWaitAfter)
{
    DestroyThreadAsync();
	
	if(nWaitAfter == 0)
		return;
	
	wxMilliSleep(nWaitAfter);
	
    while (IsThreadRun())
	{
		wxMilliSleep(nWaitAfter);
	}	
}

bool wxGISThreadHelper::TestDestroy()
{    
    wxCriticalSectionLocker locker(m_critSection);
    if (m_thread)
        return m_thread->TestDestroy() || m_bKill;
    return m_bKill;
}

void wxGISThreadHelper::KillThread()
{
    wxCriticalSectionLocker locker(m_critSection);
    if (m_thread)
    {
        m_thread->Kill();

        if (m_kind == wxTHREAD_JOINABLE)
            delete m_thread;

        m_thread = NULL;
    }
}

//------------------------------------------------------------------------------
// Class wxGISProcess
//------------------------------------------------------------------------------

wxGISProcess::wxGISProcess(IGISProcessParent* pParent, wxThreadKind kind) : wxProcess(wxPROCESS_REDIRECT), wxGISThreadHelper(kind)
{
    m_pParent = pParent;
    m_dtBeg = wxDateTime::Now();
    m_dtEstEnd = wxDateTime::Now();
    m_pid = 0;
}

wxGISProcess::~wxGISProcess(void)
{
}

bool wxGISProcess::Start()
{
	m_pid = Execute();
    if (m_pid == 0)
    {
        m_nState = enumGISTaskError;
        return false;
    }        

	m_nState = enumGISTaskWork;
	m_dtBeg = wxDateTime::Now();
    m_dfDone = 0;
	//create and start read thread stdin
	if(IsInputOpened())
	{
        return CreateAndRunThread();
	}

    //start err thread ?
    return true; //or false?
}

void wxGISProcess::OnTerminate(int pid, int status)
{
    DestroyThreadAsync();
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
            m_pParent->OnFinish(this, status == EXIT_FAILURE);
    }

	m_nState = status == EXIT_SUCCESS ? enumGISTaskDone : enumGISTaskError;
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
            wxKillError eErr = wxKILL_ERROR;
            char nKillCounter = 0;
            //try kill 3 times
            while (eErr != wxKILL_OK && nKillCounter < 3)
            {
                eErr  = Kill(m_pid, wxSIGKILL);// wxSIGINT wxSIGTERM
                nKillCounter++;
            }

            if (eErr == wxKILL_OK)
            {
                m_pid = 0;

                DestroyThreadAsync();
	            //and detach
		        Detach();
            }
            else
            {
                wxASSERT_MSG(0, wxT("Incorrect case"));
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
        AddInfo(enumGISMessageNormal, sRest);
		return;
	}
	if( sInputData.StartsWith(wxT("ERR: "), &sRest) )
	{
        AddInfo(enumGISMessageError, sRest);
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

    AddInfo(enumGISMessageUnknown, sInputData);
}

wxThread::ExitCode wxGISProcess::Entry()
{
    // IMPORTANT:
    // this function gets executed in the secondary thread context!

    // here we do our long task, periodically calling TestDestroy():

    //wxTextInputStream (wxInputStream &stream, const wxString &sep=" \t", const wxMBConv &conv=wxConvAuto())
    wxInputStream &InStream = *GetInputStream();
	wxTextInputStream InputStr(InStream, wxT(" \t"), *wxConvCurrent);
    while (!TestDestroy())
    {
        if (InStream.Eof())
            break;
        if(InStream.IsOk())
        {
            while (InStream.CanRead())
            {
                wxString line = InputStr.ReadLine();
                if (line.IsEmpty())
                    break;
                ProcessInput(line);
            }		    
        }
		wxThread::Sleep(READ_LINE_DELAY);
    }

    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}



