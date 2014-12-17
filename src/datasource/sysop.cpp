/******************************************************************************
 * Project:  wxGIS
 * Purpose:  system operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/datasource/sysop.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"

#include <wx/filename.h>
#include <wx/fontmap.h>
#include <wx/tokenzr.h>

bool DeleteDir(const CPLString &sPath, ITrackCancel* const pTrackCancel)
{
	//test if symlink
	if(IsSymlink(sPath))
	{		
		return DeleteFile(sPath, pTrackCancel);
	}

    int result = CPLUnlinkTree(sPath);
    //int result = VSIRmdir(sPath);
    if (result == -1)
    {
        if(pTrackCancel)
        {
            wxString sErr = wxString::Format(_("Delete folder failed! Folder '%s'"), wxString::FromUTF8(sPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        }
        return false;
    }
    if(pTrackCancel)
    {
        wxString sMsg = wxString::Format(_("Delete folder succeeded! Folder '%s'"), wxString::FromUTF8(sPath));
        wxGISLogMessage(sMsg, pTrackCancel);
    }
    return true;
}

bool CreateDir(const CPLString &sPath, long mode, ITrackCancel* const pTrackCancel)
{
    if( VSIMkdir( sPath, mode ) != 0 )
    {
        if(pTrackCancel)
        {
            wxString sErr = wxString::Format(_("Create folder failed! Folder '%s'"), wxString::FromUTF8(sPath));			
            wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        }
        return false;
    }
    if(pTrackCancel)
    {
        wxString sMsg = wxString::Format(_("Create folder succeeded! Folder '%s'"), wxString::FromUTF8(sPath));
		wxGISLogMessage(sMsg, pTrackCancel);
    }
#ifdef __WINDOWS__    
    if (wxGISEQUALN(CPLGetFilename(sPath), ".", 1))
    {
        SetFileAttributes(wxString::FromUTF8(sPath), FILE_ATTRIBUTE_HIDDEN);
    }
#endif
    return true;
}

bool MoveDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel)
{
    if(wxGISEQUAL(sPathFrom, sPathTo))
        return true;

#ifdef __WINDOWS__
    if(!EQUALN(sPathTo,"/vsi",4) && EQUALN(sPathFrom, sPathTo, 3))
    {
        //if in same disc - copy/rename
        return RenameFile(sPathFrom, sPathTo, pTrackCancel);
    }
#endif //__WINDOWS__
    else
    {
        //if in different discs - copy/move
        bool bRes = CopyDir(sPathFrom, sPathTo, mode, pTrackCancel);
        if(bRes)
            DeleteDir(sPathFrom);
        return bRes;
    }
    return false;
}

bool CopyDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel)
{
    if(wxGISEQUAL(sPathFrom, sPathTo))
        return true;

    if (wxGISEQUALN(sPathTo, sPathFrom, CPLStrnlen(sPathFrom, 1024)))
    {
        if (pTrackCancel)
            pTrackCancel->PutMessage(_("Cannot copy folder inside itself"), wxNOT_FOUND, enumGISMessageError);
        return false;
    }

    if(!CPLCheckForFile((char*)sPathTo.c_str(), NULL))
    {
        if(!CreateDir(sPathTo, mode, pTrackCancel))
        {
            return false;
        }
    }

    char **papszItems = CPLReadDir(sPathFrom);
    if(papszItems == NULL)
        return true;

    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if(pTrackCancel && !pTrackCancel->Continue())
            return false;

        if( wxGISEQUAL(papszItems[i], ".") || wxGISEQUAL(papszItems[i], "..") )
            continue;

        CPLString szFullPathFrom = CPLFormFilename(sPathFrom, papszItems[i], NULL);

        VSIStatBufL BufL;
        int ret = VSIStatL(szFullPathFrom, &BufL);
        if(ret == 0)
        {
            CPLString szFullPathTo = CPLFormFilename(sPathTo, papszItems[i], NULL);
            if(VSI_ISDIR(BufL.st_mode))
		    {
                if(!CopyDir(szFullPathFrom, szFullPathTo, mode, pTrackCancel))
                    return false;
            }
            else
            {
                if(!CopyFile(szFullPathFrom,szFullPathTo, pTrackCancel))
                    return false;
            }
        }
    }
    CSLDestroy( papszItems );

    return true;
}

bool DeleteFile(const CPLString &sPath, ITrackCancel* const pTrackCancel)
{
	//test if symlink
	
    int result = VSIUnlink(sPath);
    if (result == -1)
    {
        if(pTrackCancel)
        {
            wxString sErr = wxString::Format(_("Delete file failed! File '%s'"), wxString::FromUTF8(sPath));
            wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        }
        return false;
    }
    if(pTrackCancel)
    {
        wxString sMsg = wxString::Format(_("Delete file succeeded! File '%s'"), wxString::FromUTF8(sPath));
		wxGISLogMessage(sMsg, pTrackCancel);
    }
    return true;
}

bool RenameFile(const CPLString &sOldPath, const CPLString &sNewPath, ITrackCancel* const pTrackCancel)
{
    if (CPLCheckForFile((char*)sNewPath.c_str(), NULL))
        DeleteFile(sNewPath);

	int result = VSIRename(sOldPath, sNewPath);
    if (result == -1)
    {
        if(pTrackCancel)
        {
            wxString sErr = wxString::Format(_("Rename file failed! Old path '%s', new path '%s'"), wxString::FromUTF8(sOldPath), wxString::FromUTF8(sNewPath));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        }
        return false;
    }
    if(pTrackCancel)
    {
        wxString sMsg = wxString::Format(_("Rename file succeeded! Old path '%s', new path '%s'"), wxString::FromUTF8(sOldPath), wxString::FromUTF8(sNewPath));
        wxGISLogMessage(sMsg, pTrackCancel);
    }
    return true;
}

wxString ClearExt(const wxString &sPath)
{
	return wxFileName::StripExtension(sPath);
}

wxFontEncoding GetEncodingFromCpg(const CPLString &sPath)
{
    wxFontEncoding oDefaultEnc = wxFONTENCODING_DEFAULT;// wxLocale::GetSystemEncoding();
	const char* szCPGPath = CPLResetExtension(sPath, "cpg");
    if(!CPLCheckForFile((char*)szCPGPath, NULL))
        return oDefaultEnc;

    char **papszLines = CSLLoad( szCPGPath);
    if(papszLines == NULL)
        return oDefaultEnc;

    CPLString soCodePage(papszLines[0]);
    if(soCodePage.empty())
    {
 		CSLDestroy( papszLines );
        return oDefaultEnc;
    }

    CSLDestroy( papszLines );

    return ToFontEncoding(soCodePage);
}

wxFontEncoding ToFontEncoding(const CPLString &soCodePage)
{
    wxFontEncoding oDefaultEnc = wxLocale::GetSystemEncoding();
    int nCP(-1);


    if (EQUALN(soCodePage, "LDID/", 5))
    {
        nCP = atoi(soCodePage.c_str() + 5);
        //if( nCP > 255 )
        //    nCP = -1; // don't use 0 to indicate out of range as LDID/0 is a valid one
    }
    else if (EQUALN(soCodePage, "UTF-8", 5))
        return wxFONTENCODING_UTF8;
    else if (EQUALN(soCodePage, "UTF-7", 5))
        return wxFONTENCODING_UTF7;
    else if (EQUALN(soCodePage, "UTF-16", 6))
        return wxFONTENCODING_UTF16;
    else if (EQUALN(soCodePage, "KOI8-R", 6))
        return wxFONTENCODING_KOI8;
    else if (EQUALN(soCodePage, "KOI8-U", 6))
        return wxFONTENCODING_KOI8_U;
    else
        nCP = atoi(soCodePage);

    //check encoding from code http://en.wikipedia.org/wiki/Code_page
    if (nCP < 255)
    {
        //http://www.autopark.ru/ASBProgrammerGuide/DBFSTRUC.HTM
        switch (nCP)
        {
        case 1: nCP = 437;      break;
        case 2: nCP = 850;      break;
        case 3: nCP = 1252;     break;
        case 4: nCP = 10000;    break;
        case 8: nCP = 865;      break;
        case 10: nCP = 850;     break;
        case 11: nCP = 437;     break;
        case 13: nCP = 437;     break;
        case 14: nCP = 850;     break;
        case 15: nCP = 437;     break;
        case 16: nCP = 850;     break;
        case 17: nCP = 437;     break;
        case 18: nCP = 850;     break;
        case 19: nCP = 932;     break;
        case 20: nCP = 850;     break;
        case 21: nCP = 437;     break;
        case 22: nCP = 850;     break;
        case 23: nCP = 865;     break;
        case 24: nCP = 437;     break;
        case 25: nCP = 437;     break;
        case 26: nCP = 850;     break;
        case 27: nCP = 437;     break;
        case 28: nCP = 863;     break;
        case 29: nCP = 850;     break;
        case 31: nCP = 852;     break;
        case 34: nCP = 852;     break;
        case 35: nCP = 852;     break;
        case 36: nCP = 860;     break;
        case 37: nCP = 850;     break;
        case 38: nCP = 866;     break;
        case 55: nCP = 850;     break;
        case 64: nCP = 852;     break;
        case 77: nCP = 936;     break;
        case 78: nCP = 949;     break;
        case 79: nCP = 950;     break;
        case 80: nCP = 874;     break;
        case 87: nCP = 874;     return oDefaultEnc;
        case 88: nCP = 1252;     break;
        case 89: nCP = 1252;     break;
        case 100: nCP = 852;     break;
        case 101: nCP = 866;     break;
        case 102: nCP = 865;     break;
        case 103: nCP = 861;     break;
        case 104: nCP = 895;     break;
        case 105: nCP = 620;     break;
        case 106: nCP = 737;     break;
        case 107: nCP = 857;     break;
        case 108: nCP = 863;     break;
        case 120: nCP = 950;     break;
        case 121: nCP = 949;     break;
        case 122: nCP = 936;     break;
        case 123: nCP = 932;     break;
        case 124: nCP = 874;     break;
        case 134: nCP = 737;     break;
        case 135: nCP = 852;     break;
        case 136: nCP = 857;     break;
        case 150: nCP = 10007;   break;
        case 151: nCP = 10029;   break;
        case 200: nCP = 1250;    break;
        case 201: nCP = 1251;    break;
        case 202: nCP = 1254;    break;
        case 203: nCP = 1253;    break;
        case 204: nCP = 1257;    break;
        default: break;
        }
    }
    switch (nCP)
    {
    case 28591: return wxFONTENCODING_ISO8859_1;
    case 28592: return wxFONTENCODING_ISO8859_2;
    case 28593: return wxFONTENCODING_ISO8859_3;
    case 28594: return wxFONTENCODING_ISO8859_4;
    case 28595: return wxFONTENCODING_ISO8859_5;
    case 28596: return wxFONTENCODING_ISO8859_6;
    case 28597: return wxFONTENCODING_ISO8859_7;
    case 28598: return wxFONTENCODING_ISO8859_8;
    case 28599: return wxFONTENCODING_ISO8859_9;
    case 28600: return wxFONTENCODING_ISO8859_10;
        //case 874: return wxFONTENCODING_ISO8859_11;
        // case 28602: return wxFONTENCODING_ISO8859_12;      // doesn't exist currently, but put it
    case 28603: return wxFONTENCODING_ISO8859_13;
        // case 28604: return wxFONTENCODING_ISO8859_14:     ret = 28604; break; // no correspondence on Windows
    case 28605: return wxFONTENCODING_ISO8859_15;
    case 20866: return wxFONTENCODING_KOI8;
    case 21866: return wxFONTENCODING_KOI8_U;
    case 437: return wxFONTENCODING_CP437;
    case 850: return wxFONTENCODING_CP850;
    case 852: return wxFONTENCODING_CP852;
    case 855: return wxFONTENCODING_CP855;
    case 866: return wxFONTENCODING_CP866;
    case 874: return wxFONTENCODING_CP874;
    case 932: return wxFONTENCODING_CP932;
    case 936: return wxFONTENCODING_CP936;
    case 949: return wxFONTENCODING_CP949;
    case 950: return wxFONTENCODING_CP950;
    case 1250: return wxFONTENCODING_CP1250;
    case 1251: return wxFONTENCODING_CP1251;
    case 1252: return wxFONTENCODING_CP1252;
    case 1253: return wxFONTENCODING_CP1253;
    case 1254: return wxFONTENCODING_CP1254;
    case 1255: return wxFONTENCODING_CP1255;
    case 1256: return wxFONTENCODING_CP1256;
    case 1257: return wxFONTENCODING_CP1257;
    case 20932: return wxFONTENCODING_EUC_JP;
    case 10000: return wxFONTENCODING_MACROMAN;
    case 10001: return wxFONTENCODING_MACJAPANESE;
    case 10002: return wxFONTENCODING_MACCHINESETRAD;
    case 10003: return wxFONTENCODING_MACKOREAN;
    case 10004: return wxFONTENCODING_MACARABIC;
    case 10005: return wxFONTENCODING_MACHEBREW;
    case 10006: return wxFONTENCODING_MACGREEK;
    case 10007: return wxFONTENCODING_MACCYRILLIC;
    case 10021: return wxFONTENCODING_MACTHAI;
    case 10008: return wxFONTENCODING_MACCHINESESIMP;
    case 10029: return wxFONTENCODING_MACCENTRALEUR;
    case 10082: return wxFONTENCODING_MACCROATIAN;
    case 10079: return wxFONTENCODING_MACICELANDIC;
    case 10009: return wxFONTENCODING_MACROMANIAN;
    case 65000: return wxFONTENCODING_UTF7;
    case 65001: return wxFONTENCODING_UTF8;
    default: return wxFONTENCODING_DEFAULT;
    }
    return oDefaultEnc;
}

bool IsFileHidden(const CPLString &sPath)
{
#ifdef __WINDOWS__
    wxString sTestPath(sPath, wxConvUTF8);
	DWORD dwAttrs = GetFileAttributes(sTestPath.wc_str());
    if (dwAttrs != INVALID_FILE_ATTRIBUTES)
		return dwAttrs & FILE_ATTRIBUTE_HIDDEN;
#endif
	return EQUALN(CPLGetFilename(sPath), ".", 1);
}

wxString CheckUniqName(const CPLString &sPath, const wxString &sName, const wxString &sExt, int nCounter)
{
    wxString sResultName = sName + (nCounter > 0 ? wxString::Format(wxT("_%d"), nCounter) : wxString(wxT("")));
    CPLString szBaseName = (char*)CPLFormFilename(sPath, sResultName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    if(CPLCheckForFile((char*)szBaseName.c_str(), NULL))
        return CheckUniqName(sPath, sName, sExt, nCounter + 1);
    else
        return sResultName;
}

CPLString CheckUniqPath(const CPLString &sPath, const CPLString &sName, bool bIsFolder, const CPLString &sAdd, int nCounter)
{
    CPLString sResultName;
    if(nCounter > 0)
    {
        CPLString szAdd;
        szAdd.Printf("%s(%d)", sAdd.c_str(), nCounter);
        CPLString szTmpName = CPLGetBasename(sName) + szAdd;
        if(bIsFolder)
        {
            sResultName = CPLString(CPLFormFilename(sPath, szTmpName, ""));
        }
        else
        {
            sResultName = CPLString(CPLFormFilename(sPath, szTmpName, CPLGetExtension(sName)));
        }
    }
    else
    {
        sResultName = CPLString(CPLFormFilename(sPath, sName, ""));
    }

    if(CPLCheckForFile((char*)sResultName.c_str(), NULL))
        return CheckUniqPath(sPath, sName, bIsFolder, sAdd, nCounter + 1);
    else
        return sResultName;
}

CPLString GetUniqPath(const CPLString &szOriginalFullPath, const CPLString &szNewPath, const CPLString &szNewName)
{
    CPLString szNewDestFileName(CPLFormFilename("", szNewName, CPLGetExtension(szOriginalFullPath)));
    return CheckUniqPath(szNewPath, szNewDestFileName);
}

wxString Transliterate(const wxString & str)
{
    wxArrayString saFrom, saTo;
    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        saFrom = wxStringTokenize(oConfig.Read(enumGISHKCU, wxString(wxT("wxTranslit/from/values")), wxEmptyString), wxString(wxT(",")), wxTOKEN_RET_EMPTY);
        saTo = wxStringTokenize(oConfig.Read(enumGISHKCU, wxString(wxT("wxTranslit/to/values")), wxEmptyString), wxString(wxT(",")), wxTOKEN_RET_EMPTY);
    }

    //load translit from config
    wxString sOut;
    for (size_t i = 0; i < str.Length(); ++i)
    {		
        int nInd = saFrom.Index(str.GetChar(i));
        if (nInd == wxNOT_FOUND)
        {
            sOut += str.GetChar(i);
        }
        else
        {
            sOut += saTo[nInd];
        }
    }
    return sOut;
}

bool CopyFile(const CPLString &sSrcPath, const CPLString &sDestPath, ITrackCancel* const pTrackCancel)
{
    if(wxGISEQUAL(sDestPath, sSrcPath))
        return true;

    size_t nBufferSize = 1048576;//1024 * 1024;

    IProgressor* pProgr(NULL);
    if(pTrackCancel)
        pProgr = pTrackCancel->GetProgressor();
    if(pProgr)
    {
        VSIStatBufL sStatBuf;
        int ret = VSIStatL(sSrcPath, &sStatBuf);
        if(ret == 0)
        {
            int nRange = sStatBuf.st_size / nBufferSize;
            if(nRange < 1)
                nRange = 1;
            pProgr->SetRange(nRange);
        }
        else
        {
            pProgr->SetRange(100);
        }
    }

    VSILFILE *fpOld, *fpNew;
    GByte *pabyBuffer;
    size_t nBytesRead;
    int nRet = 0;

    CPLErrorReset();

/* -------------------------------------------------------------------- */
/*      Open old and new file.                                          */
/* -------------------------------------------------------------------- */
    fpOld = VSIFOpenL( sSrcPath, "rb" );
    if( fpOld == NULL )
    {
        wxString sErr(_("Error open input file!"));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        return false;
    }

    fpNew = VSIFOpenL( sDestPath, "wb" );
    if( fpNew == NULL )
    {
        VSIFCloseL( fpOld );

        wxString sErr(_("Error create the output file '%s'!"));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
        return false;
    }

/* -------------------------------------------------------------------- */
/*      Prepare buffer.                                                 */
/* -------------------------------------------------------------------- */
    pabyBuffer = (GByte *) CPLMalloc(nBufferSize);

/* -------------------------------------------------------------------- */
/*      Copy file over till we run out of stuff.                        */
/* -------------------------------------------------------------------- */
    int nCounter(0);
    do {
        if(pProgr)
            pProgr->SetValue(nCounter);

        nBytesRead = VSIFReadL( pabyBuffer, 1, nBufferSize, fpOld );
        if( nBytesRead < 0 )
            nRet = -1;

        if( nRet == 0
            && VSIFWriteL( pabyBuffer, 1, nBytesRead, fpNew ) < nBytesRead )
            nRet = -1;
        nCounter++;
		
		if(pTrackCancel && !pTrackCancel->Continue())
            break;
			
    } while( nRet == 0 && nBytesRead == nBufferSize );

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
    VSIFCloseL( fpNew );
    VSIFCloseL( fpOld );

    CPLFree( pabyBuffer );

    return nRet == 0;
}

bool MoveFile(const CPLString &sSrcPath, const CPLString &sDestPath, ITrackCancel* const pTrackCancel)
{
    if(wxGISEQUAL(sDestPath, sSrcPath))
        return true;

    if(wxGISEQUAL(CPLGetPath(sDestPath), CPLGetPath(sSrcPath)))
    {
        //if in same directory - make copy
        return RenameFile(sSrcPath, sDestPath, pTrackCancel);
    }
#ifdef __WINDOWS__
    else if(!EQUALN(sDestPath,"/vsi",4) && EQUALN(sDestPath, sSrcPath, 3))
    {
        //if in same disc - copy/rename
        return RenameFile(sSrcPath, sDestPath, pTrackCancel);
    }
#endif //TODO: UNIX RenameFile add support too
    else
    {
        //if in different discs - copy/move
        bool bRes = CopyFile(sSrcPath, sDestPath, pTrackCancel);
        if(bRes)
            DeleteFile(sSrcPath);
        return bRes;
    }
    return false;
}

CPLString GetExtension(const CPLString &sPath, const CPLString &sName)
{
	if(sName.empty())
		return CPLGetExtension(sPath);
	size_t found = sPath.rfind(sName);
	if( found != std::string::npos )
	{
		found += sName.length() + 1;
		if( found < sPath.size() - 1 )
			return sPath.substr(found);
	}
	return CPLGetExtension(sPath);
}

wxString GetConvName(const CPLString &szPath, bool bIsPath)
{
    wxString name;
    const char* szName(NULL);
    if(bIsPath)
        szName = CPLGetFilename(szPath);
    else
        szName = szPath;
    if( EQUALN(szPath,"/vsizip/",8) )
        name = wxString(szName, wxConvUTF8);
    else
        name = wxString(szName, wxConvUTF8);

	return name;
}

void AddFileToZip(const CPLString &szPath, void* hZIP, GByte **pabyBuffer, size_t nBufferSize, const CPLString &szPrependPath, const wxString &sCharset)
{
    int nRet = 0;
    size_t nBytesRead;
    VSILFILE *fp;

    CPLString szName;
    if (szPrependPath.empty())
    {
        szName = CPLGetFilename(szPath);
    }
    else
    {
        szName += szPrependPath;
        szName += "/";
        szName += CPLGetFilename(szPath);
    }

    szName = CPLString(wxString(szName, wxConvUTF8).mb_str(wxCSConv(sCharset)));

    fp = VSIFOpenL(szPath, "rb");
    if (fp == NULL)
        return;

    if (CPLCreateFileInZip(hZIP, szName, NULL) == CE_None)
    {
        do {
            nBytesRead = VSIFReadL(*pabyBuffer, 1, nBufferSize, fp);
            if (long(nBytesRead) < 0)
                nRet = -1;

            if (nRet == 0 && CPLWriteFileInZip(hZIP, *pabyBuffer, nBytesRead) != CE_None)
                nRet = -1;
        } while (nRet == 0 && nBytesRead == nBufferSize);

    }

    //    CPLError(CE_Failure, CPLE_FileIO, "ERROR adding %s to zip", szName);
    CPLCloseFileInZip(hZIP);
    VSIFCloseL(fp);

}

wxDateTime GetFileModificatioDate(const CPLString &szPath)
{
    VSIStatBufL BufL;
    wxDateTime dt;
    int ret = VSIStatL(szPath, &BufL);
    if (ret == 0)
    {
        dt = wxDateTime(BufL.st_mtime);
    }
    return dt;
}


bool IsSymlink(const CPLString &szPath)
{
#ifdef __UNIX__
	//test if symlink
	struct stat BufL;
	int ret = lstat(szPath, &BufL);
	if(ret == 0)
	{		
		if(VSI_ISLNK(BufL.st_mode))
		{			
			return true;
		}
	}
#endif //__UNIX__
	return false;
}