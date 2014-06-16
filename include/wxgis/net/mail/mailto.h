/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  mail supported class for Linux
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2004 Roberto Majadas
*   Copyright (C) 2005-2013 Bastien Nocera
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#pragma once

#include "wxgis/net/message.h"

#ifdef wxGIS_HAVE_GTK_INTEGRATION

typedef enum {
	MAILER_UNKNOWN,
	MAILER_EVO,
	MAILER_BALSA,
	MAILER_SYLPHEED,
	MAILER_THUNDERBIRD,
} MailerType;

/** @class wxGISMailer

    Configurate execute command for specific mailer

    @library{net}
 */

class wxGISMailer
{
public:
    wxGISMailer(void);
    virtual ~wxGISMailer(void);
    virtual bool Init();
    virtual bool Send(const wxMailMessage& message);
protected:
    virtual wxString GetEvoCmd() const;
    wxString GetEvoMailto (const wxMailMessage& message) const;
    wxString GetBalsaMailto (const wxMailMessage& message) const;
    wxString GetThunderbirdMailto (const wxMailMessage& message) const;
    wxString GetSylpheedMailto (const wxMailMessage& message) const;
    wxString EscapeStrings(const wxString &sData) const;
protected:
    MailerType m_eType;
    wxString m_sCmd;
};

#endif //wxGIS_HAVE_GTK_INTEGRATION