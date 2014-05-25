/******************************************************************************
 * Project:  wxGIS
 * Purpose:  raster operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/datasource/rasterop.h"

CPLString GetWorldFilePath(const CPLString &soPath)
{
    //1. thirst and last char from ext and third char set w (e.g. jpw)
    CPLString sExt = CPLGetExtension(soPath);
    CPLString sNewExt;
    sNewExt += sExt[0];
    sNewExt += sExt[sExt.size() - 1];
    sNewExt += 'w';
    CPLString szPath = (char*)CPLResetExtension(soPath, sNewExt);
	if(CPLCheckForFile((char*)szPath.c_str(), NULL))
		return szPath;
    //4. add wx to ext
    sNewExt += 'x';
    szPath = (char*)CPLResetExtension(soPath, sNewExt);
	if(CPLCheckForFile((char*)szPath.c_str(), NULL))
		return szPath;
    //2. wld
    szPath = (char*)CPLResetExtension(soPath, "wld");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
		return szPath;
    //3. add w to ext
    szPath = soPath + CPLString("w");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
		return szPath;
    return CPLString();
}

/*
#define OVR_MAX_LEVEL_SIZE 256

int GetOverviewLevels(wxGISRasterDatasetSPtr pwxGISRasterDataset, int* anOverviewList)
{
	int nSize = wxMin(pwxGISRasterDataset->GetHeight(), pwxGISRasterDataset->GetWidth());
	int nLevel(1);
	int nLevelCount(0);
	while(1)
	{
		nSize /= 2;
		if(nSize < OVR_MAX_LEVEL_SIZE)
		{
			if(nLevelCount == 0)
			{
				anOverviewList[nLevelCount] = nLevel;
				nLevelCount++;
			}
			break;
		}
		nLevel *= 2;
		if(nLevel != 2)
		{
			anOverviewList[nLevelCount] = nLevel;
			nLevelCount++;
		}
	}
	return nLevelCount;
}

*/