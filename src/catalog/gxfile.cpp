/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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
#include "wxgis/catalog/gxfile.h"
#include "wxgis/datasource/sysop.h"

#include "cpl_vsi.h"


//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxFile, wxGxObject)

wxGxFile::wxGxFile(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObject(oParent, soName, soPath)
{
}

wxGxFile::~wxGxFile(void)
{
}


bool wxGxFile::Delete(void)
{
    if(DeleteFile(m_sPath))
	{
		return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
}

bool wxGxFile::Rename(const wxString &sNewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(ClearExt(sNewName));

	wxString sNewPath = PathName.GetFullPath();
    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if(RenameFile(m_sPath, szNewPath))
	{
		return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
	return false;
}

bool wxGxFile::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Copy file ")) + m_sName, -1, enumGISMessageInfo);

	CPLString szFileName = CPLGetBasename(m_sPath);
	CPLString szNewDestFileName = GetUniqPath(m_sPath, szDestPath, szFileName);
    return CopyFile(szNewDestFileName, m_sPath, pTrackCancel);
}

bool wxGxFile::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Move file ")) + m_sName, -1, enumGISMessageInfo);

	CPLString szFileName = CPLGetBasename(m_sPath);
	CPLString szNewDestFileName = GetUniqPath(m_sPath, szDestPath, szFileName);
    return MoveFile(szNewDestFileName, m_sPath, pTrackCancel);
}

//--------------------------------------------------------------
//class wxGxPrjFile
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxPrjFile, wxGxFile)
wxGxPrjFile::wxGxPrjFile(wxGISEnumPrjFileType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFile(oParent, soName, soPath)
{
    m_eType = eType;
}

wxGxPrjFile::~wxGxPrjFile(void)
{
}

wxGISSpatialReference wxGxPrjFile::GetSpatialReference(void)
{
	OGRErr err = OGRERR_NONE;
    if(!m_SpatialReference.IsOk())
	{
		char **papszLines = CSLLoad( m_sPath );

		switch(m_eType)
		{
		case enumESRIPrjFile:
            {
                OGRSpatialReference* pSpaRef = new OGRSpatialReference();
                err = pSpaRef->importFromESRI(papszLines);
                if(err == OGRERR_NONE)
                    m_SpatialReference = wxGISSpatialReference(pSpaRef);
                else
                    wxDELETE(pSpaRef);
            }
			break;
		case enumSPRfile:
		case enumQPJfile:
            {
                OGRSpatialReference* pSpaRef = new OGRSpatialReference();
                err = pSpaRef->importFromWkt(papszLines);
                if(err == OGRERR_NONE)
                    m_SpatialReference = wxGISSpatialReference(pSpaRef);
                else
                    wxDELETE(pSpaRef);
            }
			break;
		//case enumSPRfile:
  //          {
  //              char *pszWKT, *pszWKT2;
  //              pszWKT = CPLStrdup(papszLines[0]);
  //              for(int i = 1; papszLines[i] != NULL; ++i )
  //              {
  //                  int npapszLinesSize = CPLStrnlen(papszLines[i], 1000);
  //                  int npszWKTSize = CPLStrnlen(pszWKT, 8000);
  //                  int nDestSize = npszWKTSize + npapszLinesSize + 1;
  //                  pszWKT = (char *)CPLRealloc(pszWKT, npszWKTSize + npapszLinesSize + 1 );
  //                  CPLStrlcat( pszWKT, papszLines[i], nDestSize );
  //              }
  //              pszWKT2 = pszWKT;
  //              err = m_pOGRSpatialReference->importFromWkt( &pszWKT2 );//.importFromWkt(papszLines);
  //              CPLFree( pszWKT );
  //          }
		//	break;
		default:
			break;
		}
        CSLDestroy( papszLines );
	}

    //err = m_OGRSpatialReference.importFromProj4("+proj=bonne +a=6371000 +es=0 +lon_0=0 +lat_1=60 +units=m +no_defs");
    //0x04e3c368 "+proj=bonne +ellps=sphere +lon_0=0 +lat_1=60 +units=m +no_defs "
    //0x04e3c368 "+proj=aitoff +lon_0=0 +lat_1=60 +x_0=0 +y_0=0 +a=6371000 +b=6371000 +units=m +no_defs "
	//if(m_OGRSpatialReference.GetEPSGGeogCS() == -1)
	//	m_OGRSpatialReference.SetWellKnownGeogCS("sphere");//WGS84

 //   err = m_OGRSpatialReference.Fixup();
	//err = m_OGRSpatialReference.Validate();
    //m_OGRSpatialReference.set
    //+over
	if(err != OGRERR_NONE)
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGxPrjFile: GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
		wxLogError(sErr);
	}

	return m_SpatialReference;
}

//--------------------------------------------------------------
//class wxGxTextFile
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTextFile, wxGxFile)
wxGxTextFile::wxGxTextFile(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFile(oParent, soName, soPath)
{
}

wxGxTextFile::~wxGxTextFile(void)
{
}
