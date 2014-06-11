


/*
 * Copyright (C) 2004 Roberto Majadas
 * Copyright (C) 2005-2013 Bastien Nocera
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more av.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Author:  Roberto Majadas <roberto.majadas@openshine.com>
 *          Bastien Nocera <hadess@hadess.net>
 */
/*
//#include <glib/gi18n.h>
//#include <glib/gstdio.h>
//#include <gio/gio.h>
//#include <wx/utils.h>

    wxString sCCD;
    GAppInfo *app_info = g_app_info_get_default_for_uri_scheme ("mailto");
    if (app_info) {
		sCCD = wxString (g_app_info_get_commandline (app_info));
		g_object_unref (app_info);
	}

    wxString wxstrLink = wxT("text/html");
wxMimeTypesManager * mtm = new wxMimeTypesManager();
wxFileType *ft = mtm->GetFileTypeFromMimeType(wxstrLink);
wxString one;
if (ft->GetMimeType(&one))
            wxLogDebug(one);
        wxString sOpen,sPrint;
        if(ft->GetOpenCommand(&sOpen, wxFileType::MessageParameters ()))
            wxLogDebug(sOpen);

   wxstrLink = wxString(wxT("message/rfc822"));
ft = mtm->GetFileTypeFromMimeType(wxstrLink);
if (ft->GetMimeType(&one))
            wxLogDebug(one);
        wxString sDesc;
        if(ft->GetDescription(&sDesc))
            wxLogDebug(sDesc);
        if(ft->GetOpenCommand(&sOpen, wxFileType::MessageParameters ()))
            wxLogDebug(sOpen);
        if(ft->GetPrintCommand(&sPrint, wxFileType::MessageParameters ()))
            wxLogDebug(sPrint);

    wxMimeTypesManager manager;
    wxFileType *filetype = manager.GetFileTypeFromMimeType(wxT("message/rfc822"));//.GetFileTypeFromExtension(wxT("eml"));//
    if(filetype)
    {
        wxString sDesc;
        if(filetype->GetDescription(&sDesc))
            wxLogDebug(sDesc);
        wxString sOpen;
        if(filetype->GetOpenCommand(&sOpen, wxFileType::MessageParameters ()))
            wxLogDebug(sOpen);
        //wxArrayString q1,q2;

        //size_t count = filetype->GetAllCommands(&q1, &q2,  wxFileType::MessageParameters ());

        //wxLogDebug(command);


#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gio/gio.h>

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

static const GOptionEntry entries[] = {
	{ "run-from-build-dir", 'b', 0, G_OPTION_ARG_NONE, &run_from_build_dir, N_("Run from build directory (ignored)"), NULL },
	{ "xid", 'x', 0, G_OPTION_ARG_INT64, &xid, N_("Use XID as parent to the send dialogue (ignored)"), NULL },
	{ G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames, N_("Files to send"), "[FILES...]" },
	{"version", 'v', 0, G_OPTION_ARG_NONE, &show_version, N_("Output version information and exit"), NULL},
	{ NULL }
};

static char *
get_evo_cmd (void)
{
	char *tmp, *retval;

	tmp = g_find_program_in_path ("evolution");
	if (tmp == NULL)
		return NULL;

	retval = g_strdup_printf ("%s --component=mail %%s", tmp);
	g_free (tmp);

	return retval;
}

static gboolean
init_mailer (NautilusSendto *nst)
{
	GAppInfo *app_info;
	char *needle;

	nst->type = MAILER_UNKNOWN;

	    wxFileName FName(m_pPage4->GetPath());
        wxMimeTypesManager manager;
        wxFileType *filetype = manager.GetFileTypeFromExtension(FName.GetExt());
        wxString command = filetype->GetOpenCommand(m_pPage4->GetPath());
        wxExecute(command);

	app_info = g_app_info_get_default_for_uri_scheme ("mailto");
	if (app_info) {
		nst->mail_cmd = g_strdup (g_app_info_get_commandline (app_info));
		g_object_unref (app_info);
	} else {
		nst->mail_cmd = NULL;
	}

	if (nst->mail_cmd == NULL || *nst->mail_cmd == '\0') {
		g_free (nst->mail_cmd);
		nst->mail_cmd = get_evo_cmd ();
		nst->type = MAILER_EVO;
	} else {
		/* Find what the default mailer is *//*
		if (strstr (nst->mail_cmd, "balsa"))
			nst->type = MAILER_BALSA;
		else if (strstr (nst->mail_cmd, "thunder") || strstr (nst->mail_cmd, "seamonkey") || strstr (nst->mail_cmd, "icedove")) {
			char **strv;

			nst->type = MAILER_THUNDERBIRD;

			/* Thunderbird sucks, see
			 * https://bugzilla.gnome.org/show_bug.cgi?id=614222 *//*
			strv = g_strsplit (nst->mail_cmd, " ", -1);
			g_free (nst->mail_cmd);
			nst->mail_cmd = g_strdup_printf ("%s %%s", strv[0]);
			g_strfreev (strv);
		} else if (strstr (nst->mail_cmd, "sylpheed") || strstr (nst->mail_cmd, "claws"))
			nst->type = MAILER_SYLPHEED;
		else if (strstr (nst->mail_cmd, "anjal"))
			nst->type = MAILER_EVO;
	}

	if (nst->mail_cmd == NULL)
		return FALSE;

	/* Replace %U by %s *//*
	while ((needle = g_strrstr (nst->mail_cmd, "%U")) != NULL)
		needle[1] = 's';
	while ((needle = g_strrstr (nst->mail_cmd, "%u")) != NULL)
		needle[1] = 's';

	return TRUE;
}

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

int main (int argc, char **argv)
{
	GOptionContext *context;
	GError *error = NULL;
	NautilusSendto *nst;
	int ret = 0;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	context = g_option_context_new ("");
	g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
	if (g_option_context_parse (context, &argc, &argv, &error) == FALSE) {
		g_print (_("Could not parse command-line options: %s\n"), error->message);
		g_error_free (error);
		return 1;
	}

        if (show_version) {
		g_print ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
		return 0;
	}

	nst = g_new0 (NautilusSendto, 1);
	nautilus_sendto_init (nst);
	if (!init_mailer (nst)) {
		g_print (_("No mail client installed, not sending files\n"));
		goto out;
	}


	if (nst->file_list == NULL) {
		g_print (_("Expects URIs or filenames to be passed as options\n"));
		ret = 1;
		goto out;
	}

	send_files (nst);

out:
	g_free (nst);

	return ret;
}
*/

