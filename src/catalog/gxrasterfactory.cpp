/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2013  Bishop
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
                    pGxObj = GetGxObject(pParent, GetConvName(szPath), szPath, raster_exts[j].eType);
                    if(pGxObj != NULL)
                        pChildrenIds.Add(pGxObj->GetId());
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
                unsigned int j;
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

wxGxObject* wxGxRasterFactory::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumRasterDatasetType type)
{
#ifdef CHECK_DUBLES
    if(IsNameExist(pParent, soName))
    {
        return NULL;
    }
#endif // CHECK_DUBLES

    wxGxRasterDataset* pDataset = new wxGxRasterDataset(type, pParent, soName, szPath);
    return wxStaticCast(pDataset, wxGxObject);
}
