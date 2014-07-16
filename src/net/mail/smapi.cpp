/////////////////////////////////////////////////////////////////////////////
// Name:        smapi.cpp
// Purpose:     Simple MAPI classes
// Author:      PJ Naughter <pjna@naughter.com>
// Modified by: Julian Smart
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) PJ Naughter
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wxgis/net/mail/smapi.h"
#include "wx/string.h"

#ifdef __WINDOWS__

#include "wx/msw/private.h"

// mapi.h in Cygwin's include directory isn't a full implementation and is
// not sufficient for this lib. However recent versions of Cygwin also
// have another mapi.h in include/w32api which can be used.
//

wxMapiSession::wxMapiSession()
{
    m_data = new wxMapiData;

    Initialise();
}

wxMapiSession::~wxMapiSession()
{
    //Logoff if logged on
    Logoff();

    //Unload the MAPI dll
    Deinitialise();

    delete m_data;
}

void wxMapiSession::Initialise()
{
    //First make sure the "WIN.INI" entry for MAPI is present aswell
    //as the MAPI32 dll being present on the system
    bool bMapiInstalled = (GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0) &&
        (SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0);

    if (bMapiInstalled)
    {
        //Load up the MAPI dll and get the function pointers we are interested in
        m_data->m_hMapi = ::LoadLibrary(_T("MAPI32.DLL"));
        if (m_data->m_hMapi)
        {
            m_data->m_lpfnMAPILogon = (LPMAPILOGON) GetProcAddress(m_data->m_hMapi, "MAPILogon");
            m_data->m_lpfnMAPILogoff = (LPMAPILOGOFF) GetProcAddress(m_data->m_hMapi, "MAPILogoff");
            m_data->m_lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(m_data->m_hMapi, "MAPISendMail");
            m_data->m_lpfnMAPIResolveName = (LPMAPIRESOLVENAME) GetProcAddress(m_data->m_hMapi, "MAPIResolveName");
            m_data->m_lpfnMAPIFreeBuffer = (LPMAPIFREEBUFFER) GetProcAddress(m_data->m_hMapi, "MAPIFreeBuffer");

            //If any of the functions are not installed then fail the load
            if (m_data->m_lpfnMAPILogon == NULL ||
                m_data->m_lpfnMAPILogoff == NULL ||
                m_data->m_lpfnMAPISendMail == NULL ||
                m_data->m_lpfnMAPIResolveName == NULL ||
                m_data->m_lpfnMAPIFreeBuffer == NULL)
            {
                wxLogDebug(_T("Failed to get one of the functions pointer in MAPI32.DLL\n"));
                Deinitialise();
            }
        }
    }
    else
        wxLogDebug(_T("Mapi is not installed on this computer\n"));
}

void wxMapiSession::Deinitialise()
{
    if (m_data->m_hMapi)
    {
        //Unload the MAPI dll and reset the function pointers to NULL
        FreeLibrary(m_data->m_hMapi);
        m_data->m_hMapi = NULL;
        m_data->m_lpfnMAPILogon = NULL;
        m_data->m_lpfnMAPILogoff = NULL;
        m_data->m_lpfnMAPISendMail = NULL;
        m_data->m_lpfnMAPIResolveName = NULL;
        m_data->m_lpfnMAPIFreeBuffer = NULL;
    }
}

bool wxMapiSession::Logon(const wxString& sProfileName, const wxString& sPassword, wxWindow* pParentWnd)
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPILogon); //Function pointer must be valid

    //Initialise the function return value
    bool bSuccess = FALSE;

    //Just in case we are already logged in
    Logoff();

    //Setup the ascii versions of the profile name and password
    int nProfileLength = sProfileName.Length();

    LPSTR pszProfileName = NULL;
    LPSTR pszPassword = NULL;
    if (nProfileLength)
    {
        const char *cbProfile = sProfileName.mb_str();
        const char *cbPassword = sPassword.mb_str();
        pszProfileName = (char*)cbProfile;
        pszPassword = (char*)cbPassword;
    }

    //Setup the flags & UIParam parameters used in the MapiLogon call
    FLAGS flags = 0;
    ULONG nUIParam = 0;
    /*
    if (nProfileLength == 0)
    {
        //No profile name given, then we must interactively request a profile name
        if (pParentWnd)
        {
            nUIParam = (ULONG) (HWND) pParentWnd->GetHWND();
            flags |= MAPI_LOGON_UI;
        }
        else
        {
            //No window given, just use the main window of the app as the parent window
            if (wxTheApp->GetTopWindow())
            {
                nUIParam = (ULONG) (HWND) wxTheApp->GetTopWindow()->GetHWND();
                flags |= MAPI_LOGON_UI;
            }
        }
    }
    */
    //First try to acquire a new MAPI session using the supplied settings using the MAPILogon function
    ULONG nError = m_data->m_lpfnMAPILogon(nUIParam, pszProfileName, pszPassword, flags | MAPI_NEW_SESSION, 0, &m_data->m_hSession);
    if (nError != SUCCESS_SUCCESS && nError != MAPI_E_USER_ABORT)
    {
        //Failed to create a create mapi session, try to acquire a shared mapi session
        wxLogDebug(_T("Failed to logon to MAPI using a new session, trying to acquire a shared one\n"));
        nError = m_data->m_lpfnMAPILogon(nUIParam, NULL, NULL, 0, 0, &m_data->m_hSession);
        if (nError == SUCCESS_SUCCESS)
        {
            m_data->m_nLastError = SUCCESS_SUCCESS;
            bSuccess = TRUE;
        }
        else
        {
            wxLogDebug(_T("Failed to logon to MAPI using a shared session, Error:%ld\n"), nError);
            m_data->m_nLastError = nError;
        }
    }
    else if (nError == SUCCESS_SUCCESS)
    {
        m_data->m_nLastError = SUCCESS_SUCCESS;
        bSuccess = TRUE;
    }

    return bSuccess;
}

bool wxMapiSession::LoggedOn() const
{
    return (m_data->m_hSession != 0);
}

bool wxMapiSession::MapiInstalled() const
{
    return (m_data->m_hMapi != NULL);
}

bool wxMapiSession::Logoff()
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPILogoff); //Function pointer must be valid

    //Initialise the function return value
    bool bSuccess = FALSE;

    if (m_data->m_hSession)
    {
        //Call the MAPILogoff function
        ULONG nError = m_data->m_lpfnMAPILogoff(m_data->m_hSession, 0, 0, 0);
        if (nError != SUCCESS_SUCCESS)
        {
            wxLogDebug(_T("Failed in call to MapiLogoff, Error:%ld"), nError);
            m_data->m_nLastError = nError;
            bSuccess = TRUE;
        }
        else
        {
            m_data->m_nLastError = SUCCESS_SUCCESS;
            bSuccess = TRUE;
        }
        m_data->m_hSession = 0;
    }

    return bSuccess;
}

bool wxMapiSession::Send(const wxMailMessage& message)
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPISendMail); //Function pointer must be valid
    wxASSERT(m_data->m_lpfnMAPIFreeBuffer); //Function pointer must be valid
    wxASSERT(LoggedOn()); //Must be logged on to MAPI
    wxASSERT(m_data->m_hSession); //MAPI session handle must be valid

    //Initialise the function return value
    bool bSuccess = FALSE;

    //Create the MapiMessage structure to match the message parameter send into us
    MapiMessage mapiMessage;
    ZeroMemory(&mapiMessage, sizeof(mapiMessage));

    const char* cbSubject = message.m_subject.mb_str();
    mapiMessage.lpszSubject = (char*) cbSubject;

    const char* cbBody = message.m_body.mb_str();
    mapiMessage.lpszNoteText = (char*)cbBody;

    // If we have a 'From' field, use it
    if (!message.m_from.IsEmpty())
    {
        mapiMessage.lpOriginator = new MapiRecipDesc;
        ZeroMemory(mapiMessage.lpOriginator, sizeof(MapiRecipDesc));

        mapiMessage.lpOriginator->ulRecipClass = MAPI_ORIG;


        const char* cbOriginator = message.m_from.mb_str();
        mapiMessage.lpOriginator->lpszName = (char*) cbOriginator;
    }

    //Setup the attachments
    int nAttachmentSize = message.m_attachments.GetCount();
    int nTitleSize = message.m_attachmentTitles.GetCount();
    if (nTitleSize)
    {
        wxASSERT(nTitleSize == nAttachmentSize); //If you are going to set the attachment titles then you must set
        //the attachment title for each attachment
    }
    if (nAttachmentSize)
    {
        mapiMessage.nFileCount = nAttachmentSize;
        mapiMessage.lpFiles = new MapiFileDesc[nAttachmentSize];
        for (int i = 0; i < nAttachmentSize; ++i)
        {
            MapiFileDesc& file = mapiMessage.lpFiles[i];
            ZeroMemory(&file, sizeof(MapiFileDesc));
            file.nPosition = 0xFFFFFFFF;
            wxString sFilename = message.m_attachments[i];

            const char *pszPathName = sFilename.mb_str();
            file.lpszPathName = (char*)pszPathName;
            file.lpszFileName = NULL;

            if (nTitleSize && !message.m_attachmentTitles[i].IsEmpty())
            {
                wxString sTitle = message.m_attachmentTitles[i];
                const char *pszFileName = sTitle.mb_str();
                file.lpszFileName = (char*)pszFileName;
            }
        }
    }

    //Do the actual send using MAPISendMail
    ULONG nError = m_data->m_lpfnMAPISendMail(m_data->m_hSession, 0, &mapiMessage, MAPI_DIALOG, 0);
    if (nError == SUCCESS_SUCCESS)
    {
        bSuccess = TRUE;
        m_data->m_nLastError = SUCCESS_SUCCESS;
    }
    else
    {
        wxLogDebug(_T("Failed to send mail message, Error:%ld\n"), nError);
        m_data->m_nLastError = nError;
    }

    //Tidy up the Attachements
    if (nAttachmentSize)
    {
        wxDELETEA( mapiMessage.lpFiles);
    }

    return bSuccess;
}

long wxMapiSession::GetLastError() const
{
    return m_data->m_nLastError;
}

#endif // __WINDOWS__
