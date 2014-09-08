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
#include "wxgis/core/app.h"
#include "wxgis/core/config.h"

#ifdef wxGIS_USE_CURL

#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/ffile.h>

IMPLEMENT_CLASS(wxGISCurl, wxObject)

wxGISCurl::wxGISCurl(const wxString & proxy, const wxString & sHeaders, int dnscachetimeout, int timeout, int conntimeout)
{
	m_refData = new wxGISCurlRefData(proxy, sHeaders, dnscachetimeout, timeout, conntimeout);
}

wxGISCurl::wxGISCurl(bool bReplaceUserAgent)
{
    int nDNSCacheTimeout = 180;
    int nTimeout = 1000;
    int nConnTimeout = 30;
    wxString sProxy, sHeaders;

    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        sProxy = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/curl/proxy"), wxEmptyString);
        sHeaders = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/curl/headers"), wxEmptyString);
        nDNSCacheTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/dns_cache_timeout"), nDNSCacheTimeout);
        nTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/timeout"), nTimeout);
        nConnTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/connect_timeout"), nConnTimeout);
    }

    if(bReplaceUserAgent)
    {
        wxString sUserAgent;
        IApplication *pApp = GetApplication();
        if(NULL != pApp)
        {
            sUserAgent = pApp->GetAppName() + wxT(" (") + pApp->GetAppDisplayName() + wxT(" - ") + pApp->GetAppVersionString() + wxT(")");
        }

        int pos = 0;
        if((pos = sHeaders.Find(wxT("User-Agent"))) != wxNOT_FOUND)
        {
            wxString sNewHeaders = sHeaders.Left(pos);
            wxString sSearchHeaders = sHeaders.Right(sHeaders.Len() - pos - 10);
            pos = sSearchHeaders.Find(wxT("|"));
            sNewHeaders += wxT("User-Agent: ") + sUserAgent;
            sNewHeaders += sSearchHeaders.Right(sSearchHeaders.Len() - pos);
            sHeaders = sNewHeaders;
        }
        else
        {
            sHeaders += wxT("|") + sUserAgent;
        }
    }

	m_refData = new wxGISCurlRefData(sProxy, sHeaders, nDNSCacheTimeout, nTimeout, nConnTimeout);
}

wxGISCurl::~wxGISCurl(void)
{
}

wxObjectRefData *wxGISCurl::CreateRefData() const
{
    int nDNSCacheTimeout = 180;
    int nTimeout = 1000;
    int nConnTimeout = 30;
    wxString sProxy, sHeaders;

    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        sProxy = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/curl/proxy"), wxEmptyString);
        sHeaders = oConfig.Read(enumGISHKCU, wxT("wxGISCommon/curl/headers"), wxEmptyString);
        nDNSCacheTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/dns_cache_timeout"), nDNSCacheTimeout);
        nTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/timeout"), nTimeout);
        nConnTimeout = oConfig.ReadInt(enumGISHKCU, wxT("wxGISCommon/curl/connect_timeout"), nConnTimeout);
    }

    return new wxGISCurlRefData(sProxy, sHeaders, nDNSCacheTimeout, nTimeout, nConnTimeout);
}

wxObjectRefData *wxGISCurl::CloneRefData(const wxObjectRefData *data) const
{
    return new wxGISCurlRefData(*(wxGISCurlRefData *)data);
}

bool wxGISCurl::operator == ( const wxGISCurl& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxGISCurlRefData*)m_refData == *(wxGISCurlRefData*)obj.m_refData );
}

bool wxGISCurl::IsOk() const
{
    return m_refData != NULL && ((wxGISCurlRefData *)m_refData)->m_pCurl != NULL ;
}

void wxGISCurl::SetSSLVersion(long nVer)
{
    ((wxGISCurlRefData *)m_refData)->SetSSLVersion(nVer);
}

void wxGISCurl::AppendHeader(const wxString & sHeadStr)
{
    ((wxGISCurlRefData *)m_refData)->AppendHeader(sHeadStr);
}

void wxGISCurl::SetDefaultHeader(void)
{
    ((wxGISCurlRefData *)m_refData)->SetDefaultHeader();
}

void wxGISCurl::FollowLocation(bool bSet, unsigned short iMaxRedirs)
{
    ((wxGISCurlRefData *)m_refData)->FollowLocation(bSet, iMaxRedirs);
}

PERFORMRESULT wxGISCurl::Get(const wxString & sURL)
{
    return ((wxGISCurlRefData *)m_refData)->Get(sURL);
}

bool wxGISCurl::GetFile(const wxString & sURL, const wxString & sPath)
{
    return ((wxGISCurlRefData *)m_refData)->GetFile(sURL, sPath);
}

PERFORMRESULT wxGISCurl::Post(const wxString & sURL, const wxString & sPostData)
{
    return ((wxGISCurlRefData *)m_refData)->Post(sURL, sPostData);
}

//-----------------------------------------------------------------------------
// wxGISCurlRefData
//-----------------------------------------------------------------------------
wxGISCurlRefData::wxGISCurlRefData(const wxString & proxy, const wxString & sHeaders, int dnscachetimeout, int timeout, int conntimeout)
{
    m_bUseProxy = false;
    slist = NULL;

    bodystruct.memory = NULL;
    headstruct.memory = NULL;

	m_pCurl = curl_easy_init();
	if(m_pCurl)
	{
		m_sHeaders = sHeaders;

        SetDefaultHeader();

		curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(m_pCurl, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(m_pCurl, CURLOPT_DNS_CACHE_TIMEOUT, dnscachetimeout);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEHEADER ,&headstruct);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA ,&bodystruct);
		curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, conntimeout);
        curl_easy_setopt(m_pCurl, CURLOPT_ACCEPT_ENCODING, "");
        //curl_easy_setopt(m_pCurl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        //curl_easy_setopt(m_pCurl, CURLOPT_CAPATH, "D:\\work\\projects\\wxGIS\\build32\\Debug\\sys\\ssl\\certs");
#ifdef _DEBUG
//        curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 1);
#endif
		if(!proxy.IsEmpty())
		{
			curl_easy_setopt(m_pCurl, CURLOPT_PROXY, (const char*)proxy.mb_str());
			m_bUseProxy = true;
		}
	}
}

wxGISCurlRefData::~wxGISCurlRefData(void)
{
	free(bodystruct.memory);
	free(headstruct.memory);

	curl_slist_free_all(slist);

	curl_easy_cleanup(m_pCurl);
}

wxGISCurlRefData::wxGISCurlRefData(const wxGISCurlRefData& data) : wxObjectRefData()
{
    slist = data.slist;
    m_pCurl = data.m_pCurl;
    m_sHeaders = data.m_sHeaders;
    m_bUseProxy = data.m_bUseProxy;

    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEHEADER ,&headstruct);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA ,&bodystruct);

    bodystruct.memory = NULL;
    headstruct.memory = NULL;
}

bool wxGISCurlRefData::operator == (const wxGISCurlRefData& data) const
{
    return m_pCurl == data.m_pCurl;
}

void wxGISCurlRefData::SetSSLVersion(long nVer)
{
    curl_easy_setopt(m_pCurl, CURLOPT_SSLVERSION, nVer);
}

void wxGISCurlRefData::AppendHeader(const wxString & sHeadStr)
{
	slist = curl_slist_append(slist, sHeadStr.mb_str());
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, slist);
}


void wxGISCurlRefData::SetDefaultHeader(void)
{
	curl_slist_free_all(slist);
	slist = NULL;

	wxStringTokenizer tkz(m_sHeaders, wxT("|"), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		slist = curl_slist_append(slist, token.mb_str());
	}
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, slist);
}

void wxGISCurlRefData::FollowLocation(bool bSet, unsigned short iMaxRedirs)
{
	if(bSet)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(m_pCurl, CURLOPT_MAXREDIRS, iMaxRedirs);
	}
	else
		curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 0);
}

PERFORMRESULT wxGISCurlRefData::Get(const wxString & sURL)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, (const char*)sURL.mb_str());
	//curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, FALSE);

    headstruct.size = 0;
	bodystruct.size = 0;

	res = curl_easy_perform(m_pCurl);

	//second try
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(m_pCurl);
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
		result.IsValid = true;
	}
	return result;
}

bool wxGISCurlRefData::GetFile(const wxString & sURL, const wxString & sPath)
{
	if(wxFileName::FileExists(sPath))
		return true/*false*/;
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, (const char*)sURL.mb_str());
	//curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);

	headstruct.size = 0;
	bodystruct.size = 0;

	res = curl_easy_perform(m_pCurl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(m_pCurl);
	if(res == CURLE_OK)
	{
		wxFile file(sPath, wxFile::write);
		if(file.IsOpened())
		{
			file.Write(bodystruct.memory, bodystruct.size);
			file.Close();
			return true;
		}
	}
	return false;
}

PERFORMRESULT wxGISCurlRefData::Post(const wxString & sURL, const wxString & sPostData)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(m_pCurl, CURLOPT_URL, (const char*)sURL.mb_str());
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	//const wxWX2MBbuf tmp_buf = wxConvCurrent->cWX2MB(sPostData);
	//const char *tmp_str = (const char*) tmp_buf;
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS , (const char*)sPostData.mb_str());
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, -1);
	//curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);

	headstruct.size = 0;
	bodystruct.size = 0;

    res = curl_easy_perform(m_pCurl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(m_pCurl);
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
            result.sBody = wxString((const char*)bodystruct.memory, *wxConvCurrent, bodystruct.size);
		result.iSize = headstruct.size + bodystruct.size;

		result.IsValid = true;
	}
	return result;
}

#endif //wxGIS_USE_CURL
