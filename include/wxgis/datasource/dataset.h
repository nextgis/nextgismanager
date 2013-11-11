/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#pragma once

#include "wxgis/datasource/gdalinh.h"

/** \class wxGISDataset dataset.h
    \brief The base class for datasets.
*/
class WXDLLIMPEXP_GIS_DS wxGISDataset : 
    public wxObject,
    public wxGISPointer
{
    DECLARE_ABSTRACT_CLASS(wxGISDataset)
public:
	wxGISDataset(const CPLString &sPath = "");
	virtual ~wxGISDataset(void);
	virtual wxGISEnumDatasetType GetType(void) const;
    virtual int GetSubType(void) const;
    virtual void SetSubType(int nSubType);
	virtual CPLString GetPath(void) const;
    virtual size_t GetSubsetsCount(void) const;
    virtual wxString GetName(void) const;
	virtual void Close(void);
	virtual bool IsOpened(void) const;
	virtual bool IsReadOnly(void) const;
	virtual bool IsCached(void) const;    
	virtual bool IsCaching(void) const;    
    virtual wxGISDataset* GetSubset(size_t nIndex);
	virtual const wxGISSpatialReference GetSpatialReference(void);
	virtual bool Delete(ITrackCancel* const pTrackCancel = NULL);
	virtual bool Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);    
	virtual bool CanDelete(void);
	virtual bool CanRename(void);
    virtual bool CanCopy(const CPLString &szDestPath);
	virtual bool CanMove(const CPLString &szDestPath);
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL) = 0;
    virtual void StopCaching(void);
    virtual char **GetFileList() = 0;
protected:
    virtual CPLString FixPathSeparator(CPLString &szPath);
protected:
	CPLString m_sPath;
    wxCriticalSection m_CritSect;
    int m_nSubType;
	wxGISEnumDatasetType m_nType;
	bool m_bIsOpened, m_bIsReadOnly, m_bIsCached;
    wxGISSpatialReference m_SpatialReference;
};

