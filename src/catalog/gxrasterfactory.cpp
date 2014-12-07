/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxrasterfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/sysop.h"

//------------------------------------------------------------------------------
// wxGxRasterFactory
//------------------------------------------------------------------------------
static const char *raster_add_exts[] = {
    "aux", "rrd", "ovr", "w", "wld", "bpw", "bmpw", "gifw", "jpgw", "jpegw", "pngw", "pngw", "mgrd", "sgrd", "vrtw", "vtw", "tilw", "tlw", "sdatw", "stw", NULL
};

typedef struct _rformat
{
    const char* sExt;
    wxGISEnumRasterDatasetType eType;
    const char* sDriverName;
    bool bAvailable;
} RFORMAT;

static RFORMAT raster_exts[] = {
    { "bmp",    enumRasterBmp,  "BMP",    false},
    { "jpg",    enumRasterJpeg, "JPEG",   false},
    { "jpeg",   enumRasterJpeg, "JPEG",   false},
    { "img",    enumRasterImg,  "HFA",    false},
    { "gif",    enumRasterGif,  "GIF",    false},
    { "sdat",   enumRasterSAGA, "SAGA",   false},
    { "tif",    enumRasterTiff, "GTiff",  false},
    { "tiff",   enumRasterTiff, "GTiff",  false},
    { "png",    enumRasterPng,  "PNG",    false},
    { "til",    enumRasterTil,  "TIL",    false},
    { "vrt",    enumRasterVRT,  "VRT",    false}
};

IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactory, wxGxObjectFactory)

wxGxRasterFactory::wxGxRasterFactory(void) : wxGxObjectFactory()
{
    for(int j = 0; j < sizeof(raster_exts) / sizeof(raster_exts[0]); ++j)
    {
        if(GDALGetDriverByName(raster_exts[j].sDriverName))
        {
            raster_exts[j].bAvailable = true;
        }
    }
}

wxGxRasterFactory::~wxGxRasterFactory(void)
{
}

bool wxGxRasterFactory::GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds)
{
    bool bCheckNames = CSLCount(pFileNames) < CHECK_DUBLES_MAX_COUNT;
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;
        bool bContinue(false);       

        unsigned int j;
        for(j = 0; j < sizeof(raster_exts) / sizeof(raster_exts[0]); ++j)
        {
            if(wxGISEQUAL(szExt, raster_exts[j].sExt) )
		    {
                if(raster_exts[j].bAvailable)
                {
                    CPLString szPath(pFileNames[i]);
                    pGxObj = GetGxObject(pParent, GetConvName(szPath), szPath, raster_exts[j].eType, bCheckNames);
                    if(pGxObj != NULL)
                        pChildrenIds.Add(pGxObj->GetId());
                    
                    // raster dataset container specific
                    if (raster_exts[j].eType == enumRasterTil)
                    {
                        IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObj);
                        if (pGxDataset)
                        {
                            wxGISDataset* pDSet = pGxDataset->GetDataset(false);
                            if (pDSet)
                            {
                                wxGxObjectContainer* pParentContainer = wxDynamicCast(pParent, wxGxObjectContainer);
                                char** papszFileList = pDSet->GetFileList();
                                for (size_t k = 0; k < CSLCount(papszFileList); ++k)
                                {
                                    wxString sTestName = wxString::FromUTF8(CPLGetFilename(papszFileList[k]));
                                    if (pParentContainer->IsNameExist(sTestName))
                                    {
                                        wxGxObjectList::iterator iter;
                                        wxGxObjectList children = pParentContainer->GetChildren();
                                        for (iter = children.begin(); iter != children.end(); ++iter)
                                        {
                                            wxGxObject *current = *iter;
                                            if (current && current->GetName().IsSameAs(sTestName, false))
                                            {
                                                current->Destroy();
                                                break;
                                            }
                                        }
                                    }
                                    
                                    for (int n = 0; n < i; ++n)
                                    {
                                        if (wxGISEQUAL(pFileNames[n], papszFileList[k]))
                                        {
                                            pFileNames = CSLRemoveStrings(pFileNames, n, 1, NULL);
                                            i--;
                                            break;
                                        }
                                    }
                                }                                
                                CSLDestroy(papszFileList);
                            }
                            wsDELETE(pDSet);                                                  
                        }
                    }
                }
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                bContinue = true;
                break;
		    }
        }

        if(bContinue)
            continue;


        if(wxGISEQUAL(szExt, "prj"))
        {
			if(pFileNames)
			{
                for(j = 0; j < sizeof(raster_exts) / sizeof(raster_exts[0]); ++j)
                {
    				szPath = (char*)CPLResetExtension(pFileNames[i], raster_exts[j].sExt);
	    			if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                    {
		    			pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                        bContinue = true;
                        break;
                    }
                }
            }
        }

        if(bContinue)
            continue;
        
        for( j = 0; raster_add_exts[j] != NULL; ++j )
        {
            if(wxGISEQUAL(szExt, raster_add_exts[j]))
            {
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
                break;
            }
        }
    }
	return true;
}

wxGxObject* wxGxRasterFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumRasterDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxObject *pRet = NULL;
    switch (type)
    {
    case enumRasterTil:
        {
            wxGxRasterDatasetContainer* pDataset = new wxGxRasterDatasetContainer(type, pParent, soName, szPath);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    default:
        {
            wxGxRasterDataset* pDataset = new wxGxRasterDataset(type, pParent, soName, szPath);
            pRet = wxStaticCast(pDataset, wxGxObject);
        }
        break;
    }
    
    return pRet;
}
