/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  cURL class. This is smart class for cURL handler
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
#pragma once

#include "wxgisdefs.h"

#ifdef wxGIS_USE_CURL

#include "wxgis/net/net.h"

#include <curl/curl.h>
#include <stdlib.h>

typedef struct _perform_result
{
	wxString sHead, sBody;
	unsigned int iSize;
	bool IsValid;
} PERFORMRESULT;

/** @class wxGISCurl

    A smart class for cURL handler

    @library{net}
  */

class WXDLLIMPEXP_GIS_NET wxGISCurl
{
public:
	wxGISCurl(const wxString &proxy, const wxString &sHeaders, int dnscachetimeout, int timeout, int conntimeout);
	virtual ~wxGISCurl(void);
	bool IsValid(void){return m_bIsValid;};
private:
	wxString m_sHeaders;
	bool m_bIsValid, m_bUseProxy;
	CURL *curl;
	CURLcode res;
    struct MemoryStruct
	{
      char *memory;
      size_t size;
    };
	struct curl_slist *slist;
	struct MemoryStruct bodystruct;
	struct MemoryStruct headstruct;

	static void *myrealloc(void *ptr, size_t size)
	{
		/* There might be a realloc() out there that doesn't like reallocing
		   NULL pointers, so we take care of it here */
		if (ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	}

	static size_t write_data(void *ptr, size_t size, size_t nmemb, void *data)
	{
           size_t realsize = size * nmemb;
           struct MemoryStruct *mem = (struct MemoryStruct *)data;

           mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
           if (mem->memory)
		   {
             memcpy(&(mem->memory[mem->size]), ptr, realsize);
             mem->size += realsize;
             mem->memory[mem->size] = 0;
           }
           return realsize;
	}

public:
	virtual void SetRestrictions(const wxString & sRestrict);
	virtual void AppendHeader(const wxString & sHeadStr);
	virtual void SetDefaultHeader(void);
	virtual void FollowLocation(bool bSet, unsigned short iMaxRedirs);
	virtual void SetSSLVersion(long nVer = CURL_SSLVERSION_SSLv3);
	virtual PERFORMRESULT Get(const wxString & sURL);
	virtual bool GetFile(const wxString & sURL, const wxString & sPath);
	virtual PERFORMRESULT Post(const wxString & sURL, const wxString & sPostData);
};

#endif //wxGIS_USE_CURL
