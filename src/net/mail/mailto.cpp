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
#include "wxgis/net/mail/mailto.h"

#ifdef wxGIS_HAVE_GTK_INTEGRATION

#include <gio/gio.h>

//-------------------------------------------------------------------------------------
// wxGISMailer
//-------------------------------------------------------------------------------------

wxGISMailer::wxGISMailer()
{
    m_eType = MAILER_UNKNOWN;
}

wxGISMailer::~wxGISMailer()
{

}

bool wxGISMailer::Init()
{
    wxString sCommandLine;
    GAppInfo *app_info = g_app_info_get_default_for_uri_scheme ("mailto");
    if (app_info) {
		sCommandLine = wxString (g_app_info_get_commandline (app_info));
		g_object_unref (app_info);
	}

    if(sCommandLine.IsEmpty())
    {
        m_eType = MAILER_EVO;
        m_sCmd = GetEvoCmd();
    }
    else if(sCommandLine.Find(wxT("balsa")) != wxNOT_FOUND)
    {
        m_eType = MAILER_BALSA;
        m_sCmd = sCommandLine;
    }
    else if(sCommandLine.Find(wxT("thunder")) != wxNOT_FOUND || sCommandLine.Find(wxT("seamonkey")) != wxNOT_FOUND || sCommandLine.Find(wxT("icedove")) != wxNOT_FOUND)
    {
        m_eType = MAILER_THUNDERBIRD;
        m_sCmd = sCommandLine;
    }
    else if(sCommandLine.Find(wxT("sylpheed")) != wxNOT_FOUND || sCommandLine.Find(wxT("claws")) != wxNOT_FOUND)
    {
        m_eType = MAILER_SYLPHEED;
        m_sCmd = sCommandLine;
    }
    else if(sCommandLine.Find(wxT("anjal")) != wxNOT_FOUND)
    {
        m_eType = MAILER_EVO;
        m_sCmd = sCommandLine;
    }

    m_sCmd.Replace(wxT("%u"), wxT("%s"));
    m_sCmd.Replace(wxT("%U"), wxT("%s"));

    return !m_sCmd.IsEmpty();
}

wxString wxGISMailer::GetEvoCmd() const
{
    wxString sRet;
    char* tmp = g_find_program_in_path ("evolution");
	if (tmp != NULL)
    {
        sRet = wxString::Format(wxT("%s --component=mail %%s"), tmp);
    }
    g_free (tmp);

    return sRet;
}

bool wxGISMailer::Send(const wxMailMessage& message)
{
    wxString sParam;
    switch (m_eType) {
	case MAILER_BALSA:
		sParam = GetBalsaMailto(message);
		break;
	case MAILER_SYLPHEED:
		sParam = GetSylpheedMailto(message);
		break;
	case MAILER_THUNDERBIRD:
		sParam = GetThunderbirdMailto(message);
		break;
	case MAILER_EVO:
	default:
		sParam = GetEvoMailto(message);
	}

	wxString sExec = wxString::Format(m_sCmd, sParam);
	wxExecute(sExec);

	return true;
}

wxString wxGISMailer::GetEvoMailto (const wxMailMessage& message) const
{
    wxString sRet(wxT("mailto:"));
    if(message.m_to.GetCount() > 0)
    {
        for(size_t i = 0; i < message.m_to.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT("\"%s\""), message.m_to[i].c_str());
            if(i < message.m_to.GetCount() - 1)
            {
                sRet.Append(wxT(";"));
            }
        }
    }
    else
    {
        sRet.Append(wxT("\"\""));
    }

    if(message.m_attachments.GetCount() > 0)
    {
        sRet += wxString::Format(wxT("?attach=\"%s\""), EscapeStrings(message.m_attachments[0]).c_str());
        for(size_t i = 1; i < message.m_attachments.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT("&attach=\"%s\""), EscapeStrings(message.m_attachments[i]).c_str());
        }
    }

    return sRet;
}

wxString wxGISMailer::GetBalsaMailto (const wxMailMessage& message) const
{
    wxString sRet;
    if(m_sCmd.Find(wxT(" -m ")) == wxNOT_FOUND && m_sCmd.Find(wxT(" --compose=")) == wxNOT_FOUND )
    {
        sRet.Append(wxT(" --compose="));
    }

    if(message.m_to.GetCount() > 0)
    {
        for(size_t i = 0; i < message.m_to.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT("\"%s\""), message.m_to[i].c_str());
            if(i < message.m_to.GetCount() - 1)
            {
                sRet.Append(wxT(";"));
            }
        }
    }
    else
    {
        sRet.Append(wxT("\"\""));
    }

    if(message.m_attachments.GetCount() > 0)
    {
        for(size_t i = 0; i < message.m_attachments.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT(" --attach=\"%s\""), EscapeStrings(message.m_attachments[i]).c_str());
        }
    }

    return sRet;
}

wxString wxGISMailer::GetThunderbirdMailto (const wxMailMessage& message) const
{
    wxString sRet(wxT("-compose \""));

    if(message.m_to.GetCount() > 0)
    {
        sRet.Append(wxT("to='"));
        for(size_t i = 0; i < message.m_to.GetCount(); ++i)
        {
            sRet += message.m_to[i];
            if(i < message.m_to.GetCount() - 1)
            {
                sRet.Append(wxT(","));
            }
        }
        sRet.Append(wxT("',"));
    }

    sRet += wxString::Format(wxT("subject='%s',body='%s'"), message.m_subject.c_str(), message.m_body.c_str());

    if(message.m_attachments.GetCount() > 0)
    {
        sRet.Append(wxT(",attachment='"));
        for(size_t i = 0; i < message.m_attachments.GetCount(); ++i)
        {
            sRet += message.m_attachments[i];
            if(i < message.m_attachments.GetCount() - 1)
            {
                sRet.Append(wxT(","));
            }
        }
        sRet.Append(wxT("'"));
    }

    sRet.Append(wxT("\""));
    return sRet;
}

wxString wxGISMailer::GetSylpheedMailto (const wxMailMessage& message) const
{
    wxString sRet(wxT("--compose "));

   if(message.m_to.GetCount() > 0)
    {
        for(size_t i = 0; i < message.m_to.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT("\"%s\""), message.m_to[i].c_str());
            if(i < message.m_to.GetCount() - 1)
            {
                sRet.Append(wxT(";"));
            }
        }
    }
    else
    {
        sRet.Append(wxT("\"\""));
    }

    if(message.m_attachments.GetCount() > 0)
    {
        sRet += wxString::Format(wxT("--attach \"%s\""), EscapeStrings(message.m_attachments[0]).c_str());
        for(size_t i = 1; i < message.m_attachments.GetCount(); ++i)
        {
            sRet += wxString::Format(wxT(" \"%s\""), EscapeStrings(message.m_attachments[i]).c_str());
        }
    }

    return sRet;
}

wxString wxGISMailer::EscapeStrings(const wxString &sData) const
{
    wxString sRet = sData;
    sRet.Replace(wxT("&"), wxT("%26"));
    sRet.Replace(wxT(","), wxT("%2C"));
    sRet.Replace(wxT(" "), wxT("%20"));

    return sRet;
}

#endif // wxGIS_HAVE_GTK_INTEGRATION


