/******************************************************************************
 * Project:  wxGIS (GIS common)
 * Purpose:  wxGIS config modification application.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/conf/confmain.h"

#include "wx/defs.h"
#include "wx/cmdline.h"
#include "wx/filename.h"
#include "wx/tokenzr.h"

int main(int argc, char **argv)
{
    return EXIT_SUCCESS;

}
/*
int main(int argc, char **argv)
{
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];

    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else
    #define wxArgv argv
#endif // wxUSE_UNICODE
	
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        wxFprintf(stderr, _("Failed to initialize the wxWidgets library, aborting."));
        return -1;
    }

	bool success( false );

    // Create the commandline parser
    static const wxCmdLineEntryDesc my_cmdline_desc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", _( "Show this help message" ), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "v", "version", _( "The version of this program" ) },
		{ wxCMD_LINE_OPTION, "a", "add", _( "Add data to path in XML Config." ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_SWITCH, "d", "del", _( "Delete data from XML Config" ) },
		{ wxCMD_LINE_OPTION, "n", "name", _( "The XMLNode name attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "p", "path", _( "The XMLNode path attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "e", "is_enabled", _( "The XMLNode is_enabled attribute. is_enabled choice: 1 for enabled or 0 for disabled" ), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, "c", "class", _( "The XMLNode class attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "t", "type", _( "The XMLNode type attribute. Type choice: sep, cmd, menu" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "s", "sub_type", _( "The XMLNode sub_type attribute" ), wxCMD_LINE_VAL_NUMBER },
		{ wxCMD_LINE_OPTION, "o", "cmd_name", _( "The XMLNode cmd_name attribute" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "x", "attributes", _( "The XMLNode attributes <attr_name>=<attr_value> separated by comma" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "enc", "encode", _( "encode password" ), wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_NONE }
    };

    wxCmdLineParser my_parser( my_cmdline_desc, argc, wxArgv );
    my_parser.SetLogo(wxString::Format(_("The wxGISConf (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2011"), APP_VER));
    // Parse command line arguments
    success = parse_commandline_parameters( my_parser );

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wxArgv[n]);

        delete [] wxArgv;
    }
#endif // wxUSE_UNICODE

    wxUnusedVar(argc);
    wxUnusedVar(argv);	
    
    return success == true ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool parse_commandline_parameters( wxCmdLineParser& parser )
{
    // Parse the parameters
    int my_parse_success = parser.Parse( );
    // Print help if the specify /? or if a syntax error occured.
    if( my_parse_success != 0 )
    {
        return false;
    }

	if( parser.Found( wxT( "v" ) ) )
	{
	    wxString out = wxString::Format(_("The wxGISConf (%s)\nAuthor: Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\nCopyright (c) 2011\n"), APP_VER);
	    wxFprintf(stdout, out);
		return true;
	}
	wxString sPass;
	if( parser.Found( wxT( "enc" ), &sPass) )
	{
		wxString sCryptPasswd = Encode(sPass, CONFIG_DIR);
		sCryptPasswd.Append(wxT("\n"));
	    wxFprintf(stdout, sCryptPasswd);
		return true;
	}

	wxString sPathInConfig;
	if( parser.Found( wxT( "a" ), &sPathInConfig) )
	{
		wxString sName, sPath, sClass, sType, sCmdName, sAttributes;
		long nIsEnabled, nSubType;//, nIndex(-1)

		bool bNameSet = parser.Found( wxT( "n" ), &sName );
		bool bPathSet = parser.Found( wxT( "p" ), &sPath );
		bool bClassSet = parser.Found( wxT( "c" ), &sClass );
		bool bTypeSet = parser.Found( wxT( "t" ), &sType );
		bool bCmdNameSet = parser.Found( wxT( "o" ), &sCmdName );

		bool bIsEnabledSet = parser.Found( wxT( "e" ), &nIsEnabled );
		//bool bIndexSet = parser.Found( wxT( "i" ), &nIndex );
		bool bSubtypeSet = parser.Found( wxT( "s" ), &nSubType );
		bool bAttributesSet = parser.Found( wxT( "x" ), &sAttributes );

		wxGISAppConfigSPtr pConfig;
		wxArrayString aTokens = wxStringTokenize(sPathInConfig, wxString(wxT("/")));
		wxXmlNode* pNode = GetConfigNode(aTokens, pConfig);
		if(!pNode)
			return false;

		if(bNameSet)
			AddAttribute(pNode, wxT("name"), sName);
		if(bNameSet)
			AddAttribute(pNode, wxT("name"), sName);
		if(bPathSet)
			AddAttribute(pNode, wxT("path"), sPath);
		if(bClassSet)
			AddAttribute(pNode, wxT("class"), sClass);
		if(bTypeSet)
			AddAttribute(pNode, wxT("type"), sType);
		if(bCmdNameSet)
			AddAttribute(pNode, wxT("cmd_name"), sCmdName);
		if(bIsEnabledSet)
			AddAttribute(pNode, wxT("is_enabled"), nIsEnabled);
		if(bSubtypeSet)
			AddAttribute(pNode, wxT("sub_type"), nSubType);
		if(bAttributesSet)
			AddProperties(pNode, sAttributes);

        pConfig->Clean(true);
		return true;
	}

	if( parser.Found( wxT( "d" )) )
	{
		return false;
	}

	parser.Usage();

    // Either we are using the defaults or the provided parameters were valid.

    return true;

} 


void AddAttribute(wxXmlNode* pNode, wxString sParamName, wxString sParamValue)
{
	if(pNode->HasAttribute(sParamName))
		pNode->DeleteAttribute(sParamName);
	pNode->AddAttribute(sParamName, sParamValue);
}

void AddAttribute(wxXmlNode* pNode, wxString sParamName, long nParamValue)
{
	if(pNode->HasAttribute(sParamName))
		pNode->DeleteAttribute(sParamName);
	pNode->AddAttribute(sParamName, wxString::Format(wxT("%d"), nParamValue));
}

void AddProperties(wxXmlNode* pNode, wxString sAttributes)
{
	wxStringTokenizer tkz(sAttributes, wxString(wxT(",")), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		int nPos = wxNOT_FOUND;
		if((nPos = token.Find(wxT("="))) != wxNOT_FOUND)
		{
			wxString sAttrName = token.Left(nPos);
            nPos++;
			wxString sAttrValue = token.Right(token.Len() - nPos);
			if(sAttrName.IsEmpty())
				continue;
			AddAttribute(pNode, sAttrName, sAttrValue);
		}
	}
}

wxXmlNode* GetConfigNode(wxArrayString& aTokens, wxGISAppConfigSPtr& pConfig)
{
	if(aTokens.GetCount() < 2)
		return NULL;
	pConfig = boost::make_shared<wxGISAppConfig>(aTokens[0], CONFIG_DIR);
	wxXmlNode* pRootNode = pConfig->GetConfigNode(enumGISHKLM, wxEmptyString);
	if(!pRootNode)
		return NULL;

	wxXmlNode* pChildNode = pRootNode->GetChildren();
    wxXmlNode* pParentNode = pRootNode;
	wxString sPath, sAttrCmp, sAttrName, sAttrValue;
	for (size_t i = 1; i < aTokens.GetCount(); ++i )
	{
        wxString token = aTokens[i];
		//search # in name
		int nPos = wxNOT_FOUND;
		if(( nPos = token.Find(wxT("#"))) != wxNOT_FOUND)
		{
			sPath = token.Left(nPos);
            nPos++;
			sAttrCmp = token.Right(token.Len() - nPos);
            aTokens[i] = sPath;
		}
        else
            sPath = token;

		if(( nPos = sAttrCmp.Find(wxT("="))) != wxNOT_FOUND)
		{
			sAttrName = sAttrCmp.Left(nPos);
            nPos++;
			sAttrValue = sAttrCmp.Right(sAttrCmp.Len() - nPos);
		}

        if(pChildNode == NULL)
        {
            pChildNode = new wxXmlNode(pParentNode, wxXML_ELEMENT_NODE, sPath);
            if(!sAttrName.IsEmpty() && !sAttrValue.IsEmpty())
                pChildNode->AddAttribute(sAttrName, sAttrValue);
            pParentNode = pChildNode;
            continue;
        }

        while(pChildNode)
        {
			wxString sChildName = pChildNode->GetName();
            if(sPath.CmpNoCase(sChildName) == 0)
			{
				if(sAttrName.IsEmpty() && sAttrValue.IsEmpty())
				{
                    if(i < aTokens.GetCount() - 1)
                    {
                        pParentNode = pChildNode;
                        pChildNode = pChildNode->GetChildren();
                    }
					break;
				}
				else
				{
					if(pChildNode->GetAttribute(sAttrName, wxT("")).CmpNoCase(sAttrValue) == 0)
					    break;
				}
			}
            pChildNode = pChildNode->GetNext();
        }
    }
    if(pChildNode == NULL)
    {
        pChildNode = new wxXmlNode(pParentNode, wxXML_ELEMENT_NODE, sPath);
        if(!sAttrName.IsEmpty() && !sAttrValue.IsEmpty())
            pChildNode->AddAttribute(sAttrName, sAttrValue);
    }

	//if(pChildNode->GetName().CmpNoCase(aTokens[aTokens.GetCount() - 1]) == 0)
    return pChildNode;
    //return NULL;
}
*/
