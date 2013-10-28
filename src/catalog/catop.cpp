/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  catalog operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/catop.h"
#include "wxgis/catalog/gxcatalog.h"

bool OverWriteGxObject(wxGxObject* const pGxObject, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(pGxObject, true, wxT("The GxObject pointer is NULL"));

    IGxObjectEdit* pGxDstObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObject);
    if(pGxDstObjectEdit)
    {
        //add messages to pTrackCancel
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Overwrite destination object"), -1, enumGISMessageInfo);
		if(!pGxDstObjectEdit->CanDelete() || !pGxDstObjectEdit->Delete())
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Failed overwrite destination object"), -1, enumGISMessageErr);
			return false;
		}
    }
	else
	{
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Cannot delete item!"), -1, enumGISMessageErr);
        return false;
	}
	return true;
}

wxGxObject* GetParentGxObjectFromPath(const wxString &sFullPath, ITrackCancel* const pTrackCancel)
{
    wxFileName sDstFileName(sFullPath);
    wxString sPath = sDstFileName.GetPath();

    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if(pCatalog)
    {
        wxGxObject* pGxDstObject = pCatalog->FindGxObject(sPath);
        if(!pGxDstObject)
        {
            if(pTrackCancel)
                pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        }
        return pGxDstObject;
    }
    return NULL;
}
