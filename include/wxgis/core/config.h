/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISConfig class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Dmitry Baryshnikov
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

#include "wxgis/core/core.h"

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/hashmap.h>
#include <wx/xml/xml.h>

WX_DECLARE_STRING_HASH_MAP( wxXmlNode*, wxGISConfigNodesMap );

/** @class wxGISConfig

    The config main class. This is the wrapper around xml config files. wxGISConfig cached all opened xml config files for speed. All changes are stored to appropriate files before wxGISConfig class destructs on program exit.

    @library {core}
*/
class WXDLLIMPEXP_GIS_CORE wxGISConfig : public wxObject
{
    DECLARE_CLASS(wxGISConfig)
public:
    wxGISConfig(void);
	wxGISConfig(bool bPortable);
    void Create(bool bPortable = false);
    void ReportPaths(void);

    bool IsOk() const { return m_refData != NULL; };

    bool operator == ( const wxGISConfig& obj ) const;
    bool operator != (const wxGISConfig& obj) const { return !(*this == obj); };

    bool Delete(wxGISEnumConfigKey Key, const wxString &sPath);

	wxString Read(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sDefaultValue);
	int ReadInt(wxGISEnumConfigKey Key, const wxString &sPath, int nDefaultValue);
	double ReadDouble(wxGISEnumConfigKey Key, const wxString &sPath, double dDefaultValue);
	bool ReadBool(wxGISEnumConfigKey Key, const wxString &sPath, bool bDefaultValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, bool bValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, int nValue);

	static void DeleteNodeChildren(wxXmlNode* pNode);
	wxXmlNode *GetConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);
	wxXmlNode *CreateConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);

	wxString GetLocalConfigDir(void) const;
	wxString GetGlobalConfigDir(void) const;
	wxString GetLocalConfigDirNonPortable(void) const;

    bool Save(const wxGISEnumConfigKey Key = enumGISHKAny);
	wxString GetConfigDir(const wxString& wxDirName) const;
protected:
	bool SplitPathToXml(const wxString &  fullpath, wxString *psFileName, wxString *psPathInXml);
	bool SplitPathToAttribute(const wxString &  fullpath, wxString *psPathToAttribute, wxString *psAttributeName);
	wxXmlNode* GetConfigRootNode(wxGISEnumConfigKey Key, const wxString &sFileName) const;
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};

/** @class wxGISConfigRefData

    The reference data class for wxGISConfig

    @library{core}
*/

class  wxGISConfigRefData : public wxObjectRefData
{
    friend class wxGISConfig;
    friend class wxGISAppConfig;
public:
    wxGISConfigRefData();
    virtual ~wxGISConfigRefData();
    bool Save(const wxGISEnumConfigKey Key = enumGISHKAny, const wxString&  sXmlFileName = wxEmptyString);
    wxGISConfigRefData( const wxGISConfigRefData& data );
    bool operator == (const wxGISConfigRefData& data) const;

	//typedefs
	typedef struct wxxmlconf
	{
		wxXmlDocument *pXmlDoc;
        wxGISEnumConfigKey eKey;
        wxString sXmlFileName;
		wxString sXmlFilePath;
	}WXXMLCONF;

protected:
	wxString m_sLocalConfigDirPath, m_sGlobalConfigDirPath, m_sAppExeDirPath;
	wxString m_sLocalConfigDirPathNonPortable;
    bool m_bPortable;
	wxVector<WXXMLCONF> m_paConfigFiles;
	wxGISConfigNodesMap m_pmConfigNodes;
protected:
    wxCriticalSection m_oCritSect;
};

/** @class wxGISAppConfig

    The extended config class.
	Added methods for Get/Set Locale, System Directory and Log Directory paths, Debug mode

	@library{core}
*/

class WXDLLIMPEXP_GIS_CORE wxGISAppConfig : public wxGISConfig
{
    DECLARE_CLASS(wxGISAppConfig);
public:
    wxGISAppConfig(void);
	wxGISAppConfig(bool bPortable);

    wxString GetLocale(void);
    wxString GetLocaleDir(void);
    wxString GetSysDir(void);
    wxString GetTempDir(void);
    wxString GetLogDir(void);
    bool GetDebugMode(void);
    void SetLocale(const wxString &sLocale);
    void SetLocaleDir(const wxString &sLocaleDir);
    void SetSysDir(const wxString &sSysDir);
	void SetLogDir(const wxString &sLogDir);
    void SetDebugMode(bool bDebug);
};

/** @fn wxGISAppConfig GetConfig(void)

    Global config getter.
	If config object is not exist it created, otherwise - AddRef to smart pointer

	@library{core}
 */
WXDLLIMPEXP_GIS_CORE wxGISAppConfig GetConfig(void);

