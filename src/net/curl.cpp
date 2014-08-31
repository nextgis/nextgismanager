/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  cURL class. This is smart clas for cURL handler
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008,2010-2012 Dmitry Baryshnikov
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
#include "wxgis/net/curl.h"
#include "wxgisdefs.h"

#ifdef wxGIS_USE_CURL

#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/ffile.h>

wxGISCurl::wxGISCurl(const wxString & proxy, const wxString & sHeaders, int dnscachetimeout, int timeout, int conntimeout) :
m_bIsValid(false), m_bUseProxy(false), slist(NULL)
{
	curl = curl_easy_init();
	if(curl)
	{
		bodystruct.size = 0;
		bodystruct.memory = NULL;
		headstruct.size = 0;
		headstruct.memory = NULL;

		m_sHeaders = sHeaders;

        SetDefaultHeader();

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, dnscachetimeout);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER ,&headstruct);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA ,&bodystruct);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, conntimeout);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        //curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        //curl_easy_setopt(curl, CURLOPT_CAPATH, "D:\\work\\projects\\wxGIS\\build32\\Debug\\sys\\ssl\\certs");
#ifdef _DEBUG
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif
		if(!proxy.IsEmpty())
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, (const char*)proxy.mb_str());
			m_bUseProxy = true;
		}
		m_bIsValid = true;
	}
}

wxGISCurl::~wxGISCurl(void)
{
	free(bodystruct.memory);
	free(headstruct.memory);
	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
}

void wxGISCurl::SetSSLVersion(long nVer)
{
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, nVer);
}

void wxGISCurl::AppendHeader(const wxString & sHeadStr)
{
	slist = curl_slist_append(slist, sHeadStr.mb_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
}

void wxGISCurl::SetDefaultHeader(void)
{
	curl_slist_free_all(slist);
	slist = NULL;

	wxStringTokenizer tkz(m_sHeaders, wxT("|"), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		slist = curl_slist_append(slist, token.mb_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
}

void wxGISCurl::FollowLocation(bool bSet, unsigned short iMaxRedirs)
{
	if(bSet)
	{
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, iMaxRedirs);
	}
	else
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
}

PERFORMRESULT wxGISCurl::Get(const wxString & sURL)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)sURL.mb_str());
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
	res = curl_easy_perform(curl);

	//second try
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	//
	if(res == CURLE_OK)
	{
		result.sHead = wxString((const char*)headstruct.memory, headstruct.size);//wxConvLocal,
        //charset
        int posb = result.sHead.Find(wxT("charset="));
        wxString soSet;//(wxT("default"));
        if( posb != wxNOT_FOUND)
        {
            soSet = result.sHead.Mid(posb + 8, 50);
            int pose = soSet.Find(wxT("\r\n"));
            soSet = soSet.Left(pose);
        }

        if( soSet.IsSameAs(wxT("utf-8"), false) || soSet.IsSameAs(wxT("utf8"), false) )
        {
            result.sBody = wxString((const char*)bodystruct.memory, wxConvUTF8, bodystruct.size);
        }
        else if( soSet.IsSameAs(wxT("utf-16"), false) || soSet.IsSameAs(wxT("utf16"), false) )
        {
            result.sBody = wxString((const char*)bodystruct.memory, wxConvUTF8, bodystruct.size);
        }
        else if( soSet.IsSameAs(wxT("utf-32"), false) || soSet.IsSameAs(wxT("utf32"), false) )
        {
            result.sBody = wxString((const char*)bodystruct.memory, wxConvUTF8, bodystruct.size);
        }
        else if( soSet.IsSameAs(wxT("utf-7"), false) || soSet.IsSameAs(wxT("utf7"), false) )
        {
            result.sBody = wxString((const char*)bodystruct.memory, wxConvUTF8, bodystruct.size);
        }
        else
        {
            //wxCSConv
            wxCSConv conv(soSet);

            if(conv.IsOk())
                result.sBody = wxString((const char*)bodystruct.memory, conv, bodystruct.size);
            else
                result.sBody = wxString((const char*)bodystruct.memory, *wxConvCurrent, bodystruct.size);//wxConvLocal,
        }

		result.iSize = headstruct.size + bodystruct.size;
		headstruct.size = 0;
		bodystruct.size = 0;
		result.IsValid = true;
	}
	return result;
}

bool wxGISCurl::GetFile(const wxString & sURL, const wxString & sPath)
{
	if(wxFileName::FileExists(sPath))
		return true/*false*/;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)sURL.mb_str());
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	res = curl_easy_perform(curl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	if(res == CURLE_OK)
	{
		headstruct.size = 0;
		wxFile file(sPath, wxFile::write);
		if(file.IsOpened())
		{
			file.Write(bodystruct.memory, bodystruct.size);
			file.Close();
			bodystruct.size = 0;
			return true;
		}
		bodystruct.size = 0;
	}
	return false;
}

PERFORMRESULT wxGISCurl::Post(const wxString & sURL, const wxString & sPostData)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)sURL.mb_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	//const wxWX2MBbuf tmp_buf = wxConvCurrent->cWX2MB(sPostData);
	//const char *tmp_str = (const char*) tmp_buf;
	const char *tmp_str = sPostData.mb_str();
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS , tmp_str);
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	res = curl_easy_perform(curl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	if(res == CURLE_OK)
	{
		result.sHead = wxString((const char*)headstruct.memory, headstruct.size);//, wxConvLocal
        //charset
        int posb = result.sHead.Find(wxT("charset="));
        wxString soSet;//(wxT("default"));
        if( posb != wxNOT_FOUND)
        {
            soSet = result.sHead.Mid(posb + 8, 50);
            int pose = soSet.Find(wxT("\r\n"));
            soSet = soSet.Left(pose);
        }
        //wxCSConv
        wxCSConv conv(soSet);

        if(conv.IsOk())
            result.sBody = wxString((const char*)bodystruct.memory, conv, bodystruct.size);
        else
            result.sBody = wxString((const char*)bodystruct.memory, *wxConvCurrent, bodystruct.size);//, wxConvLocal
		result.iSize = headstruct.size + bodystruct.size;
		headstruct.size = 0;
		bodystruct.size = 0;
		result.IsValid = true;
	}
	return result;
}

#endif //wxGIS_USE_CURL
