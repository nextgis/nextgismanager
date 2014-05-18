/////////////////////////////////////////////////////////////////////////////
// Name:        smapi.h
// Purpose:     Simple MAPI classes
// Author:      PJ Naughter <pjna@naughter.com>
// Modified by: Julian Smart
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) PJ Naughter
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "wxgis/net/net.h"
#include "wxgis/net/message.h"

#include "wxgisdefs.h"

#ifdef wxGIS_USE_EMAIL

#ifdef __WINDOWS__

#ifdef __CYGWIN__
    #include <w32api/mapi.h>
#else
    #include <mapi.h>
#endif

class WXDLLIMPEXP_GIS_NET wxMapiData
{
public:
    wxMapiData()
    {
        m_hSession = 0;
        m_nLastError = 0;
        m_hMapi = NULL;
        m_lpfnMAPILogon = NULL;
        m_lpfnMAPILogoff = NULL;
        m_lpfnMAPISendMail = NULL;
        m_lpfnMAPIResolveName = NULL;
        m_lpfnMAPIFreeBuffer = NULL;
    }

    //Data
    LHANDLE             m_hSession; //Mapi Session handle
    long                m_nLastError; //Last Mapi error value
    HINSTANCE           m_hMapi; //Instance handle of the MAPI dll
    LPMAPILOGON         m_lpfnMAPILogon; //MAPILogon function pointer
    LPMAPILOGOFF        m_lpfnMAPILogoff; //MAPILogoff function pointer
    LPMAPISENDMAIL      m_lpfnMAPISendMail; //MAPISendMail function pointer
    LPMAPIRESOLVENAME   m_lpfnMAPIResolveName; //MAPIResolveName function pointer
    LPMAPIFREEBUFFER    m_lpfnMAPIFreeBuffer; //MAPIFreeBuffer function pointer
};


//The class which encapsulates the MAPI connection
class WXDLLIMPEXP_GIS_NET wxMapiSession
{
public:
    //Constructors / Destructors
    wxMapiSession(void);
    ~wxMapiSession(void);
    
    //Logon / Logoff Methods
    bool Logon(const wxString& sProfileName, const wxString& sPassword = wxEmptyString, wxWindow* pParentWnd = NULL);
    bool LoggedOn() const;
    bool Logoff();
    
    //Send a message
    bool Send(wxMailMessage& message);
    
    //General MAPI support
    bool MapiInstalled() const;
    
    //Error Handling
    long GetLastError() const;
    
protected:
    //Methods
    void Initialise();
    void Deinitialise(); 

    wxMapiData* m_data;    
};

#endif //__WINDOWS__

#endif //wxGIS_USE_EMAIL
