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

  /*

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gio/gio.h>*/

/* Options *//*
static char **filenames = NULL;
static gboolean run_from_build_dir = FALSE;
static gint64 xid = 0;
static gboolean show_version = FALSE;

typedef struct {
	GList *file_list;
	guint num_dirs;
	MailerType type;
	char *mail_cmd;
} NautilusSendto;


static char *
get_filename_from_list (GList *file_list)
{
	GList *l;
	GString *common_part = NULL;
	gboolean matches = TRUE;
	guint offset = 0;
	const char *encoding;
	gboolean use_utf8 = TRUE;

	encoding = g_getenv ("G_FILENAME_ENCODING");

	if (encoding != NULL && strcasecmp (encoding, "UTF-8") != 0)
		use_utf8 = FALSE;

	if (file_list == NULL)
		return NULL;

	common_part = g_string_new ("");

	while (TRUE) {
		gunichar cur_char = '\0';
		for (l = file_list; l ; l = l->next) {
			char *path = NULL, *name = NULL;
			char *offset_name = NULL;

			path = g_filename_from_uri ((char *) l->data, NULL, NULL);
			if (!path)
				break;

			name = g_path_get_basename (path);

			if (!use_utf8) {
				char *tmp;

				tmp = g_filename_to_utf8 (name, -1, NULL, NULL, NULL);
				g_free (name);
				name = tmp;
			}

			if (!name) {
				g_free (path);
				break;
			}

			if (offset >= g_utf8_strlen (name, -1)) {
				g_free(name);
				g_free(path);
				matches = FALSE;
				break;
			}

			offset_name = g_utf8_offset_to_pointer (name, offset);

			if (offset_name == g_utf8_strrchr (name, -1, '.')) {
				g_free (name);
				g_free (path);
				matches = FALSE;
				break;
			}
			if (cur_char == '\0') {
				cur_char = g_utf8_get_char (offset_name);
			} else if (cur_char != g_utf8_get_char (offset_name)) {
				g_free (name);
				g_free (path);
				matches = FALSE;
				break;
			}
			g_free (name);
			g_free (path);
		}
		if (matches == TRUE &&
		    cur_char != '\0' &&
		    cur_char != '-' &&
		    cur_char != '_') {
			offset++;
			common_part = g_string_append_unichar (common_part,
					cur_char);
		} else {
			break;
		}
	}

	if (g_utf8_strlen (common_part->str, -1) < 4) {
		g_string_free (common_part, TRUE);
		return NULL;
	}

	return g_string_free (common_part, FALSE);
}

static char *
pack_filename_from_names (GList *file_list)
{
	char *filename;
	gboolean one_file;

	if (file_list != NULL && file_list->next != NULL)
		one_file = FALSE;
	else if (file_list != NULL)
		one_file = TRUE;

	if (one_file) {
		char *filepath;

		filepath = g_filename_from_uri ((char *)file_list->data,
						NULL, NULL);
		filename = g_path_get_basename (filepath);
		g_free (filepath);
	} else {
		filename = get_filename_from_list (file_list);
	}

	if (filename == NULL) {
		/* Translators: the default archive name if it
		 * could not be deduced from the provided files *//*
		filename = g_strdup (_("Archive"));
	}

	return filename;
}

static char *
pack_files (GList *file_list)
{
	char *file_roller_cmd;
	const char *filename;
	GList *l;
	GString *cmd, *tmp;
	char *pack_type, *tmp_dir, *tmp_work_dir, *packed_file;

	file_roller_cmd = g_find_program_in_path ("file-roller");
	filename = pack_filename_from_names (file_list);

	g_assert (filename != NULL && *filename != '\0');

	tmp_dir = g_strdup_printf ("%s/nautilus-sendto-%s",
				   g_get_tmp_dir (), g_get_user_name ());
	g_mkdir (tmp_dir, 0700);
	tmp_work_dir = g_strdup_printf ("%s/nautilus-sendto-%s/%li",
					g_get_tmp_dir (), g_get_user_name (),
					time (NULL));
	g_mkdir (tmp_work_dir, 0700);
	g_free (tmp_dir);

	pack_type = g_strdup (".zip");

	cmd = g_string_new ("");
	g_string_printf (cmd, "%s --add-to=\"%s/%s%s\"",
			 file_roller_cmd, tmp_work_dir,
			 filename,
			 pack_type);

	/* file-roller doesn't understand URIs *//*
	for (l = file_list ; l; l=l->next){
		char *file;

		file = g_filename_from_uri (l->data, NULL, NULL);
		g_string_append_printf (cmd," \"%s\"", file);
		g_free (file);
	}

	g_spawn_command_line_sync (cmd->str, NULL, NULL, NULL, NULL);
	g_string_free (cmd, TRUE);
	tmp = g_string_new("");
	g_string_printf (tmp,"%s/%s%s", tmp_work_dir,
			 filename,
			 pack_type);
	g_free (tmp_work_dir);
	packed_file = g_filename_to_uri (tmp->str, NULL, NULL);
	g_string_free(tmp, TRUE);

	return packed_file;
}

static void
get_evo_mailto (GString         *mailto,
		GList           *file_list)
{
	GList *l;

	g_string_append (mailto, "mailto:");
	g_string_append (mailto, "\"\"");
	g_string_append_printf (mailto,"?attach=\"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next)
		g_string_append_printf (mailto,"&attach=\"%s\"", (char *)l->data);
}

static void
get_balsa_mailto (NautilusSendto  *nst,
		  GString         *mailto,
		  GList           *file_list)
{
	GList *l;

	if (strstr (nst->mail_cmd, " -m ") == NULL && strstr (nst->mail_cmd, " --compose=") == NULL)
		g_string_append (mailto, " --compose=");
	g_string_append (mailto, "\"\"");
	g_string_append_printf (mailto," --attach=\"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next)
		g_string_append_printf (mailto," --attach=\"%s\"", (char *)l->data);
}

static void
get_thunderbird_mailto (GString         *mailto,
			GList           *file_list)
{
	GList *l;

	g_string_append (mailto, "-compose \"");
	g_string_append_printf (mailto,"attachment='%s", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next)
		g_string_append_printf (mailto,",%s", (char *)l->data);
	g_string_append (mailto, "'\"");
}

static void
get_sylpheed_mailto (GString         *mailto,
		     GList           *file_list)
{
	GList *l;

	g_string_append (mailto, "--compose ");
	g_string_append (mailto, "\"\"");
	g_string_append_printf (mailto,"--attach \"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next)
		g_string_append_printf (mailto," \"%s\"", (char *)l->data);
}

static void
send_files (NautilusSendto *nst)
{
	GString *mailto;
	char *cmd;

	if (nst->num_dirs > 0) {
		char *zip;
		zip = pack_files (nst->file_list);
		g_list_free_full (nst->file_list, g_free);
		nst->file_list = g_list_append (NULL, zip);
	}

	mailto = g_string_new ("");
	switch (nst->type) {
	case MAILER_BALSA:
		get_balsa_mailto (nst, mailto, nst->file_list);
		break;
	case MAILER_SYLPHEED:
		get_sylpheed_mailto (mailto, nst->file_list);
		break;
	case MAILER_THUNDERBIRD:
		get_thunderbird_mailto (mailto, nst->file_list);
		break;
	case MAILER_EVO:
	default:
		get_evo_mailto (mailto, nst->file_list);
	}

	cmd = g_strdup_printf (nst->mail_cmd, mailto->str);
	g_string_free (mailto, TRUE);

	g_debug ("Mailer type: %d", nst->type);
	g_debug ("Command: %s", cmd);

	/* FIXME: collect errors from this call *//*
	g_spawn_command_line_async (cmd, NULL);
	g_free (cmd);
}

static char *
escape_ampersands_and_commas (const char *url)
{
	int i;
	char *str, *ptr;

	/* Count the number of ampersands & commas *//*
	i = 0;
	ptr = (char *) url;
	while ((ptr = strchr (ptr, '&')) != NULL) {
		i++;
		ptr++;
	}
	ptr = (char *) url;
	while ((ptr = strchr (ptr, ',')) != NULL) {
		i++;
		ptr++;
	}

	/* No ampersands or commas ? *//*
	if (i == 0)
		return NULL;

	/* Replace the '&' *//*
	str = g_malloc0 (strlen (url) - i + 3 * i + 1);
	ptr = str;
	for (i = 0; url[i] != '\0'; i++) {
		if (url[i] == '&') {
			*ptr++ = '%';
			*ptr++ = '2';
			*ptr++ = '6';
		} else if (url[i] == ',') {
			*ptr++ = '%';
			*ptr++ = '2';
			*ptr++ = 'C';
		} else {
			*ptr++ = url[i];
		}
	}

	return str;
}

static void
nautilus_sendto_init (NautilusSendto *nst)
{
	int i;

	/* Clean up the URIs passed, and collect whether we have directories *//*
	for (i = 0; filenames != NULL && filenames[i] != NULL; i++) {
		GFile *file;
		char *filename, *escaped, *uri;
		GFileInfo *info;
		const char *mimetype;

		/* We need a filename *//*
		file = g_file_new_for_commandline_arg (filenames[i]);
		filename = g_file_get_path (file);
		if (filename == NULL) {
			g_object_unref (file);
			continue;
		}

		/* Get the mime-type, and whether the file is readable *//*
		info = g_file_query_info (file,
					  G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE","G_FILE_ATTRIBUTE_ACCESS_CAN_READ,
					  G_FILE_QUERY_INFO_NONE,
					  NULL,
					  NULL);
		g_object_unref (file);

		if (info == NULL)
			continue;

		if (g_file_info_get_attribute_boolean (info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ) == FALSE) {
			g_debug ("'%s' is not readable", filenames[i]);
			g_object_unref (info);
			continue;
		}
		mimetype = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE);
		if (g_str_equal (mimetype, "inode/directory"))
			nst->num_dirs++;

		g_object_unref (info);

		uri = g_filename_to_uri (filename, NULL, NULL);
		g_free (filename);
		escaped = escape_ampersands_and_commas (uri);

		if (escaped == NULL) {
			nst->file_list = g_list_prepend (nst->file_list, uri);
		} else {
			nst->file_list = g_list_prepend (nst->file_list, escaped);
			g_free (uri);
		}
	}

	nst->file_list = g_list_reverse (nst->file_list);
}

*/

