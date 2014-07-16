/////////////////////////////////////////////////////////////////////////////
// Name:        email.h
// Purpose:     wxEmail: portable email client class
// Author:      Julian Smart
// Modified by:
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "wxgis/net/net.h"
#include "wxgis/net/message.h"

#include "wxgisdefs.h"

#ifdef wxGIS_USE_EMAIL

/** @class wxEmail

    Miscellaneous email functions

    @library{net}
 */

class WXDLLIMPEXP_GIS_NET wxEmail
{
public:
    wxEmail(void);

    // Send a message.
    // Specify profile, or leave it to wxWidgets to find the current user name
    static bool Send(const wxMailMessage& message);

};

#endif //wxGIS_USE_EMAIL

