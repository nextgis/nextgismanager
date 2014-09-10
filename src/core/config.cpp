/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISConfig class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
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

#include "wxgis/core/config.h"

#include <wx/tokenzr.h>

//TODO: memory leaks fix
static wxGISAppConfig g_pConfig;

extern WXDLLIMPEXP_GIS_CORE wxGISAppConfig GetConfig(void)
{
    if(g_pConfig.IsOk())
        return g_pConfig;

	#ifdef wxGIS_PORTABLE
		g_pConfig.Create(true);
	#else
		g_pConfig.Create(false);
	#endif

	return g_pConfig;
}

//---------------------------------------------------------------
// wxGISConfig
//---------------------------------------------------------------

IMPLEMENT_CLASS(wxGISConfig, wxObject)

wxGISConfig::wxGISConfig()
{
}

wxGISConfig::wxGISConfig(bool bPortable)
{
    Create(bPortable);
}

void wxGISConfig::Create(bool bPortable)
{
    wxString sVendorName = wxTheApp->GetVendorName();

    m_refData = new wxGISConfigRefData();

    wxStandardPaths::Get().UseAppInfo(wxStandardPathsBase::AppInfo_VendorName);

	((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath = wxPathOnly(wxStandardPaths::Get().GetExecutablePath());

    ((wxGISConfigRefData *)m_refData)->m_bPortable = bPortable;

    wxString sExeAppName = wxFileNameFromPath(wxStandardPaths::Get().GetExecutablePath());
	sExeAppName = wxFileName::StripExtension(sExeAppName);

    if(bPortable)
    {
        //if potable - config path: [app.exe path\config]
        ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath = ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath = ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxString(wxT("config"));
	    if(!wxDirExists(((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath))
		    wxFileName::Mkdir(((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath, 0755, wxPATH_MKDIR_FULL);
    }
    else
    {
#ifdef __WINDOWS__
        //we assume that exe is located at bin and log should be at the same directory
        ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath = wxStandardPaths::Get().GetConfigDir();
        if(((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath.Find(sExeAppName) != wxNOT_FOUND)
            ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath.Replace(sExeAppName + wxFileName::GetPathSeparator(), wxString(wxT("")));

#elif __UNIX__
        wxString sLocalConfigDirPath = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + wxString(wxT(".")) + sVendorName;
        ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath = sLocalConfigDirPath;
        wxString sGlobalConfigDirPath = wxStandardPaths::Get().GetConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath = sGlobalConfigDirPath;
#else
    #error Unsuported platform. I hope yet.
#endif
    }

#ifdef __WINDOWS__
	((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPathNonPortable = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
#elif __UNIX__
	((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPathNonPortable = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + wxString(wxT(".")) + sVendorName;
#else
    #error Unsuported platform. I hope yet.
#endif
}

wxObjectRefData *wxGISConfig::CreateRefData() const
{
    return new wxGISConfigRefData();
}

wxObjectRefData *wxGISConfig::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISConfigRefData(*(wxGISConfigRefData *)data);
}

bool wxGISConfig::operator == ( const wxGISConfig& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISConfigRefData*)m_refData == *(wxGISConfigRefData*)obj.m_refData );
}

//HKLM wxGISCatalog/Frame/Views
//HKLM wxGISCatalog/PropertyPages/Page/class

wxString wxGISConfig::Read(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sDefaultValue)
{
    wxCHECK_MSG( IsOk(), sDefaultValue, wxT("Invalid wxGISConfig") );
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return sDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return sDefaultValue;
	return pNode->GetAttribute(sAttributeName, sDefaultValue);
}

int wxGISConfig::ReadInt(wxGISEnumConfigKey Key, const wxString &sPath, int nDefaultValue)
{
    wxCHECK_MSG( IsOk(), nDefaultValue, wxT("Invalid wxGISConfig") );
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return nDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return nDefaultValue;
	return wxAtoi(pNode->GetAttribute(sAttributeName, wxString::Format(wxT("%d"), nDefaultValue)));
}

double wxGISConfig::ReadDouble(wxGISEnumConfigKey Key, const wxString &sPath, double dDefaultValue)
{
    wxCHECK_MSG( IsOk(), dDefaultValue, wxT("Invalid wxGISConfig") );
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return dDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return dDefaultValue;
	return wxAtof(pNode->GetAttribute(sAttributeName, wxString::Format(wxT("%f"), dDefaultValue)));
}

bool wxGISConfig::ReadBool(wxGISEnumConfigKey Key, const wxString &sPath, bool bDefaultValue)
{
    wxCHECK_MSG( IsOk(), bDefaultValue, wxT("Invalid wxGISConfig") );
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return bDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return bDefaultValue;
	wxString sDefaultValue = bDefaultValue == true ? wxString(wxT("yes")) : wxString(wxT("no"));
	wxString sValue = pNode->GetAttribute(sAttributeName, sDefaultValue);

	return sValue.CmpNoCase(wxString(wxT("yes"))) == 0 || sValue.CmpNoCase(wxString(wxT("on"))) == 0 || sValue.CmpNoCase(wxString(wxT("1"))) == 0 || sValue.CmpNoCase(wxString(wxT("t"))) == 0 || sValue.CmpNoCase(wxString(wxT("true"))) == 0;
}

bool wxGISConfig::SplitPathToXml(const wxString &  fullpath, wxString *psFileName, wxString *psPathInXml)
{
	int nPos = fullpath.Find('/');
	if(nPos == wxNOT_FOUND)
		return false;
	*psFileName = fullpath.Left(nPos) + wxT(".xml");
	*psPathInXml = fullpath.Right(fullpath.Len() - nPos - 1);
	return true;
}

bool wxGISConfig::SplitPathToAttribute(const wxString &  fullpath, wxString *psPathToAttribute, wxString *psAttributeName)
{
	int nPos = fullpath.Find('/', true);
	if(nPos == wxNOT_FOUND)
		return false;
	*psPathToAttribute = fullpath.Left(nPos);
	*psAttributeName = fullpath.Right(fullpath.Len() - nPos - 1);
	return true;
}
wxXmlNode *wxGISConfig::GetConfigRootNode(wxGISEnumConfigKey Key, const wxString &sFileName) const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("Invalid wxGISConfig") );

    wxXmlDocument *pXmlDoc(NULL);
    //search cached configs
	for(size_t i = 0; i < ((wxGISConfigRefData *)m_refData)->m_paConfigFiles.size(); ++i)
		if(((wxGISConfigRefData *)m_refData)->m_paConfigFiles[i].sXmlFileName.CmpNoCase(sFileName) == 0 && ((wxGISConfigRefData *)m_refData)->m_paConfigFiles[i].eKey == Key)
			return ((wxGISConfigRefData *)m_refData)->m_paConfigFiles[i].pXmlDoc->GetRoot();

	wxString sConfigDirPath;
	switch(Key)
	{
	case enumGISHKLM:
		sConfigDirPath = ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath;
		break;
	case enumGISHKCU:
		sConfigDirPath = ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath;
		break;
	default:
		return NULL;
	};

	if(!wxDirExists(sConfigDirPath))
		wxFileName::Mkdir(sConfigDirPath, 0755, wxPATH_MKDIR_FULL);

	wxString sConfigFilePath = sConfigDirPath + wxFileName::GetPathSeparator() + sFileName;
	if(wxFileName::FileExists(sConfigFilePath))
	{
        pXmlDoc = new wxXmlDocument(sConfigFilePath);
	}
	else
	{
		//Get global config if local config is not available
		if(Key == enumGISHKCU)
        {
			if(!wxDirExists(((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath))
				wxFileName::Mkdir(sConfigDirPath, 0777, wxPATH_MKDIR_FULL);
			else
			{
				wxString sConfigFilePathNew = ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath + wxFileName::GetPathSeparator() + sFileName;
				if(wxFileName::FileExists(sConfigFilePathNew))
					pXmlDoc = new wxXmlDocument(sConfigFilePathNew);
			}
		}

		//last chance - load from config directory near application executable
        if(!pXmlDoc || !pXmlDoc->IsOk())
		{
            wxFileName oFName(((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath);
            wxString sConfigFilePathNew = oFName.GetPath() + wxFileName::GetPathSeparator() + wxT("config") + wxFileName::GetPathSeparator() + sFileName;
            if(wxFileName::FileExists(sConfigFilePathNew))
				pXmlDoc = new wxXmlDocument(sConfigFilePathNew);
		}

		//create new config
		if(!pXmlDoc || !pXmlDoc->IsOk())
		{
			wxString sRootNodeName = sFileName.Left(sFileName.Len() - 4);//trim ".xml"
            pXmlDoc = new wxXmlDocument();
			pXmlDoc->SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, sRootNodeName));
		}
	}

	if(!pXmlDoc || !pXmlDoc->IsOk())
		return NULL;

	wxGISConfigRefData::WXXMLCONF conf = {pXmlDoc, Key, sFileName, sConfigFilePath};
	((wxGISConfigRefData *)m_refData)->m_paConfigFiles.push_back(conf);

    return pXmlDoc->GetRoot();
}


wxXmlNode* wxGISConfig::GetConfigNode(wxGISEnumConfigKey Key, const wxString &sPath)
{
    wxCHECK_MSG( IsOk(), NULL, wxT("Invalid wxGISConfig") );
	wxString sFullPath;
	switch(Key)
	{
	case enumGISHKLM:
		sFullPath = wxString(wxT("HKLM/"));
		break;
	case enumGISHKCU:
		sFullPath = wxString(wxT("HKCU/"));
		break;
	default:
		return  NULL;
	};

	sFullPath += sPath;
    sFullPath = sFullPath.MakeLower();

    //search cached configs nodes
	wxXmlNode* pOutputNode = ((wxGISConfigRefData *)m_refData)->m_pmConfigNodes[sFullPath];
	if(pOutputNode)
		return pOutputNode;

	//split path
	wxString sFileName, sPathInFile;
	if(!SplitPathToXml(sPath, &sFileName, &sPathInFile))
		return NULL;

	//get config root
	wxXmlNode* pRoot = GetConfigRootNode(Key, sFileName);
    if(!pRoot)
        return NULL;

	if(sPathInFile.IsEmpty())
		return pRoot;

    wxXmlNode* pChildNode = pRoot->GetChildren();

    wxStringTokenizer tkz(sPathInFile, wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			if(token.CmpNoCase(sChildName) == 0)
			{
				if(tkz.HasMoreTokens())
					pChildNode = pChildNode->GetChildren();
                break;
			}
            pChildNode = pChildNode->GetNext();
        }
    }

	if(token.CmpNoCase(sChildName) == 0)
    {
		//store pointer for speed find
		((wxGISConfigRefData *)m_refData)->m_pmConfigNodes[sFullPath] = pChildNode;
        return pChildNode;
    }
    return NULL;
}

wxXmlNode* wxGISConfig::CreateConfigNode(wxGISEnumConfigKey Key, const wxString &sPath)
{
    wxCHECK_MSG( IsOk(), NULL, wxT("Invalid wxGISConfig") );
	//split path
	wxString sFileName, sPathInFile;
	if(!SplitPathToXml(sPath, &sFileName, &sPathInFile))
		return NULL;


	//get config root
	wxXmlNode* pRoot = GetConfigRootNode(Key, sFileName);
    if(!pRoot)
        return NULL;

	wxXmlNode* pChildNode = pRoot->GetChildren();

	wxStringTokenizer tkz(sPathInFile, wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	bool bCreate(true);
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
		bCreate = true;
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			if(token.CmpNoCase(sChildName) == 0)
			{
				bCreate = false;
				if(tkz.HasMoreTokens())
				{
					pRoot = pChildNode;
					pChildNode = pChildNode->GetChildren();
				}
                break;
			}
            pChildNode = pChildNode->GetNext();
        }
		if(bCreate)
		{
            wxXmlNode *pParent = pChildNode == 0 ? pRoot : pChildNode;
			pChildNode = new wxXmlNode(pParent, wxXML_ELEMENT_NODE, token);
			if(tkz.HasMoreTokens())
			{
				pRoot = pChildNode;
				pChildNode = pChildNode->GetChildren();
			}
		}
	}
	return pChildNode;
}

void wxGISConfig::DeleteNodeChildren(wxXmlNode* pNode)
{
	wxXmlNode* pChild = pNode->GetChildren();
	while(pChild)
	{
		wxXmlNode* pDelChild = pChild;
		pChild = pChild->GetNext();
		if(pNode->RemoveChild(pDelChild))
			delete pDelChild;
	}
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sValue)
{
    wxCriticalSectionLocker locker(((wxGISConfigRefData *)m_refData)->m_oCritSect);
    //split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, bool bValue)
{
    wxCriticalSectionLocker locker(((wxGISConfigRefData *)m_refData)->m_oCritSect);
    //split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	wxString sValue = bValue == true ?  wxString(wxT("yes")) : wxString(wxT("no"));
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, int nValue)
{
    wxCriticalSectionLocker locker(((wxGISConfigRefData *)m_refData)->m_oCritSect);
    //split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	wxString sValue = wxString::Format(wxT("%d"), nValue);
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

wxString wxGISConfig::GetLocalConfigDir(void) const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    return ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath;
}

wxString wxGISConfig::GetGlobalConfigDir(void) const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    return ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath;
}

wxString wxGISConfig::GetLocalConfigDirNonPortable(void) const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    return ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPathNonPortable;
}

void wxGISConfig::Save(const wxGISEnumConfigKey Key)
{
    wxCHECK_RET( IsOk(), wxT("Invalid wxGISConfig") );
    return ((wxGISConfigRefData *)m_refData)->Save(Key);
}

void wxGISConfig::ReportPaths(void)
{
    wxLogMessage(wxT("Executable Dir: %s"), ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath.c_str());
    wxLogMessage(wxT("Local config Dir: %s"), ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPath.c_str());
    wxLogMessage(wxT("Global config Dir: %s"), ((wxGISConfigRefData *)m_refData)->m_sGlobalConfigDirPath.c_str());
}


//---------------------------------------------------------------
// wxGISConfigRefData
//---------------------------------------------------------------

wxGISConfigRefData::wxGISConfigRefData()
{
}

wxGISConfigRefData::~wxGISConfigRefData()
{
    wxGISEnumConfigKey CmpKey = enumGISHKCU;//bInstall == true ? enumGISHKLM : enumGISHKCU;
 	for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
	{
//the config state storing to files while destruction config class (smart pointer)
//on linux saving file in destructor produce segmentation fault
//#ifdef __WXMSW__
//            //Store only user settings. Common settings should be changed during install process
//            if(m_paConfigFiles[i].eKey == CmpKey)
//            {
//                wxString sXmlFilePath = m_paConfigFiles[i].sXmlFilePath;
//                m_paConfigFiles[i].pXmlDoc->Save(sXmlFilePath);
//            }
//#endif
        wxDELETE(m_paConfigFiles[i].pXmlDoc);
	}
	m_paConfigFiles.clear();
}

void wxGISConfigRefData::Save(const wxGISEnumConfigKey Key, const wxString&  sXmlFileName)
{
    wxCriticalSectionLocker locker(m_oCritSect);
    if(sXmlFileName.IsEmpty())
    {
	    for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
        {
            if(Key == enumGISHKAny || m_paConfigFiles[i].eKey & Key)
            {
                if(m_paConfigFiles[i].pXmlDoc && m_paConfigFiles[i].pXmlDoc->IsOk())
                {
                     //if(wxFileName::IsFileWritable(m_paConfigFiles[i].sXmlFilePath))
                    wxString sXmlFilePath = m_paConfigFiles[i].sXmlFilePath;
                    bool bSave = m_paConfigFiles[i].pXmlDoc->Save(sXmlFilePath);
                    if(!bSave)
                    {
                        wxLogError(_("Failed to save config '%s'"), sXmlFilePath.c_str());
                    }
                }
            }
	    }
    }
    else
    {
 	    for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
        {
            if(m_paConfigFiles[i].eKey & Key && m_paConfigFiles[i].sXmlFileName.CmpNoCase(sXmlFileName) == 0)
            {
                if(m_paConfigFiles[i].pXmlDoc && m_paConfigFiles[i].pXmlDoc->IsOk())
                {
                    m_paConfigFiles[i].pXmlDoc->Save(m_paConfigFiles[i].sXmlFilePath);
                }
            }
	    }
    }
}

wxGISConfigRefData::wxGISConfigRefData( const wxGISConfigRefData& data ) : wxObjectRefData()
{
    m_sLocalConfigDirPath = data.m_sLocalConfigDirPath;
    m_sGlobalConfigDirPath = data.m_sGlobalConfigDirPath;
    m_sLocalConfigDirPathNonPortable = data.m_sLocalConfigDirPathNonPortable;
    m_bPortable = data.m_bPortable;
    m_pmConfigNodes = data.m_pmConfigNodes;
    m_paConfigFiles = data.m_paConfigFiles;
}

bool wxGISConfigRefData::operator == (const wxGISConfigRefData& data) const
{
    return m_sLocalConfigDirPath == data.m_sLocalConfigDirPath &&
        m_sGlobalConfigDirPath == data.m_sGlobalConfigDirPath &&
        m_sLocalConfigDirPathNonPortable == data.m_sLocalConfigDirPathNonPortable &&
        m_bPortable == data.m_bPortable &&
        m_pmConfigNodes.size() == data.m_pmConfigNodes.size() &&
        m_paConfigFiles.size() == data.m_paConfigFiles.size();
}

//---------------------------------------------------------------
// wxGISAppConfig
//---------------------------------------------------------------

IMPLEMENT_CLASS(wxGISAppConfig, wxGISConfig)

wxGISAppConfig::wxGISAppConfig() : wxGISConfig()
{
}

wxGISAppConfig::wxGISAppConfig(bool bPortable) : wxGISConfig(bPortable)
{
}

wxString wxGISAppConfig::GetLocale(void)
{
    const wxLanguageInfo* loc_info = wxLocale::GetLanguageInfo(wxLocale::GetSystemLanguage());
    wxString sDefaultOut = loc_info->CanonicalName;
    if(sDefaultOut.IsEmpty())
        sDefaultOut = wxString(wxT("en"));
    else
    {
        //remove duplicated part of name
        int pos = sDefaultOut.Find(wxT("_"));
        if(pos != wxNOT_FOUND)
        {
            wxString sPart1 = sDefaultOut.Left(pos);
            wxString sPart2 = sDefaultOut.Right(sDefaultOut.Len() - pos - 1);
            if(sPart1.CmpNoCase(sPart2) == 0)
                sDefaultOut = sPart1;
        }
    }

	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/loc/locale")), sDefaultOut);
}

wxString wxGISAppConfig::GetLocaleDir(void)
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    wxString sDefaultOut;


    if (((wxGISConfigRefData *)m_refData)->m_bPortable)
    {
        sDefaultOut = ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxString(wxT("locale"));
        return sDefaultOut;
    }
    else
    {
#ifdef __WINDOWS__
        //we assume that exe is located at bin and log should be at the same directory
        wxFileName oName(((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath);
        sDefaultOut = oName.GetPath() + wxFileName::GetPathSeparator() + wxString(wxT("locale"));
#elif __UNIX__
        sDefaultOut = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxString(wxT("locale"));
#else
    #error Unsuported platform. I hope yet.
#endif
    }
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/loc/path")), sDefaultOut);
}

wxString wxGISAppConfig::GetLogDir(void)
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
	wxLogNull noLog;

    wxString sDefaultOut;
    if (((wxGISConfigRefData *)m_refData)->m_bPortable)
    {
        sDefaultOut = ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxString(wxT("log"));
    }
    else
    {
        sDefaultOut = ((wxGISConfigRefData *)m_refData)->m_sLocalConfigDirPathNonPortable + wxFileName::GetPathSeparator() + wxString(wxT("log"));
    }
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/log/path")), sDefaultOut);
}

wxString wxGISAppConfig::GetTempDir(void)
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    wxString sDefaultOut;

    if(((wxGISConfigRefData *)m_refData)->m_bPortable)
    {
        sDefaultOut = ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxString(wxT("tmp"));
        return sDefaultOut;
    }
    else
    {
       sDefaultOut = wxStandardPaths::Get().GetTempDir() + wxFileName::GetPathSeparator() + wxTheApp->GetVendorName();
    }
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/tmp/path")), sDefaultOut);
}

wxString wxGISAppConfig::GetSysDir(void)
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("Invalid wxGISConfig") );
    wxString sDefaultOut;

    if(((wxGISConfigRefData *)m_refData)->m_bPortable)
    {
        sDefaultOut = ((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxString(wxT("sys"));
        return sDefaultOut;
    }
    else
    {
#ifdef __WINDOWS__
        //we assume that exe is located at bin and log should be at the same directory
        wxFileName oName(((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath);
        sDefaultOut = oName.GetPath() + wxFileName::GetPathSeparator() + wxString(wxT("sys"));
#elif __UNIX__
        sDefaultOut = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxString(wxT("sys"));
#else
    #error Unsuported platform. I hope yet.
#endif
    }
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/sys/path")), sDefaultOut);
}

bool wxGISAppConfig::GetDebugMode(void)
{
    bool bDefaultOut = false;
	return ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/debug/mode")), bDefaultOut);
}

void wxGISAppConfig::SetLocale(const wxString &sLocale)
{
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/loc/locale")), sLocale);
}

void wxGISAppConfig::SetLocaleDir(const wxString &sLocaleDir)
{
    wxCHECK_RET( IsOk(), "Invalid wxGISConfig" );
    if(((wxGISConfigRefData *)m_refData)->m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/loc/path")), sLocaleDir);
}

void wxGISAppConfig::SetSysDir(const wxString &sSysDir)
{
    wxCHECK_RET( IsOk(), "Invalid wxGISConfig" );
    if(((wxGISConfigRefData *)m_refData)->m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/sys/path")), sSysDir);
}

void wxGISAppConfig::SetLogDir(const wxString &sLogDir)
{
    wxCHECK_RET( IsOk(), "Invalid wxGISConfig" );
    if(((wxGISConfigRefData *)m_refData)->m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/log/path")), sLogDir);
}

void wxGISAppConfig::SetDebugMode(bool bDebug)
{
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/debug/mode")), bDebug);
}
