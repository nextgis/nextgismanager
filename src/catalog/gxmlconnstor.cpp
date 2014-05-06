/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxXMLConnectionStorage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012,2013 Bishop
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

#include "wxgis/catalog/gxmlconnstor.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/format.h"

#include <wx/wfstream.h>

/*
//----------------------------------------------------------------------------
// wxGxXMLConnectionStorage
//----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxXMLConnectionStorage, wxGxObjectContainer)

wxGxXMLConnectionStorage::wxGxXMLConnectionStorage() : wxGxObjectContainer()
{
}

wxGxXMLConnectionStorage::~wxGxXMLConnectionStorage(void)
{
}

void wxGxXMLConnectionStorage::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    //reread conn.xml file
    //wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    if(event.GetPath().GetFullName().CmpNoCase(m_sXmlStorageName) == 0)
        LoadConnectionsStorage();

}

void wxGxXMLConnectionStorage::LoadConnectionsStorage(void)
{
    wxCriticalSectionLocker locker(m_oCritSect);

    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_sXmlStoragePath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();

        //check version
        int nVer = GetDecimalValue(pConnectionsNode, wxT("ver"), 1);
        if(nVer < GetStorageVersion())
        {
            DestroyChildren();
            wxGxCatalogBase* pGxCatalog = GetGxCatalog();
            if(pGxCatalog)
                pGxCatalog->ObjectChanged(GetId());
            CreateConnectionsStorage();
            return;
        }

 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

        wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
        std::map<long, bool> mnIds;

        wxGxObjectList::const_iterator iter;
        for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;
             mnIds[current->GetId()] = false;
        }

		while(pConnectionNode)
		{
            bool bFoundKey = false;

            for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                if(!current)
                    continue;

                if(IsObjectExist(current, pConnectionNode))
                {
                    bFoundKey = true;
                    mnIds[current->GetId()] = true;
                    break;
                }
            }

            if(!bFoundKey)
            {
                wxGxObject* pNewGxObject = CreateChildGxObject(pConnectionNode);
                //ObjectAdded event
                if(pGxCatalog)
                    pGxCatalog->ObjectAdded(pNewGxObject->GetId());
                mnIds[pNewGxObject->GetId()] = true;
            }

			pConnectionNode = pConnectionNode->GetNext();
		}

        if(mnIds.empty())//delete all items
        {
            if(GetChildren().GetCount() != 0)
            {
                DestroyChildren();
                if(pGxCatalog)
                    pGxCatalog->ObjectChanged(GetId());
            }
        }
        else
        {
            //delete items
            for(std::map<long, bool>::const_iterator i = mnIds.begin(); i != mnIds.end(); ++i)
            {
                if(pGxCatalog && i->second == false)
                {
                    if(DestroyChild(pGxCatalog->GetRegisterObject(i->first)))
                        pGxCatalog->ObjectDeleted(i->first);
                }
            }
        }
    }
    else
    {
        CreateConnectionsStorage();
    }
}
*/
//----------------------------------------------------------------------------
// wxGxJSONConnectionStorage
//----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxJSONConnectionStorage, wxGxObjectContainer)

wxGxJSONConnectionStorage::wxGxJSONConnectionStorage() : wxGxObjectContainer()
{
}

wxGxJSONConnectionStorage::~wxGxJSONConnectionStorage(void)
{
}

void wxGxJSONConnectionStorage::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    //reread conn.json file
    wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    if(event.GetPath().GetFullName().CmpNoCase(m_sStorageName) == 0)
        LoadConnectionsStorage();

}

void wxGxJSONConnectionStorage::LoadConnectionsStorage(void)
{
    wxCriticalSectionLocker locker(m_oCritSect);

    if(!wxFileName::FileExists(m_sStoragePath))
    {
        return CreateConnectionsStorage();
    }
    wxFileInputStream StorageInputStream(m_sStoragePath);
    wxJSONReader oStorageReader;
    wxJSONValue  oStorageRoot;
    //try to load connections json file
    int numErrorsStorage = oStorageReader.Parse( StorageInputStream, &oStorageRoot );
    if ( numErrorsStorage > 0 )  {
        const wxArrayString& errors = oStorageReader.GetErrors();
        wxString sErrMsg(_("The JSON Storage document is not well-formed"));
        for (size_t i = 0; i < errors.GetCount(); ++i)
        {
            wxString sErr = errors[i];
            sErrMsg.Append(wxT("\n"));
            sErrMsg.Append(wxString::Format(wxT("%d. %s"), i, sErr.c_str()));
        }
        wxLogError(sErrMsg);
        return CreateConnectionsStorage();
    }

    //check version
    int nVer = oStorageRoot[wxT("version")].AsInt();
    if(nVer < GetStorageVersion())
    {
        DestroyChildren();
        wxGxCatalogBase* pGxCatalog = GetGxCatalog();
        if(pGxCatalog)
            pGxCatalog->ObjectChanged(GetId());
        return CreateConnectionsStorage();
    }

    wxJSONValue oStorageConnections = oStorageRoot[wxT("connections")];

    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    std::map<long, bool> mnIds;

    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
            mnIds[current->GetId()] = false;
    }

    for( int i = 0; i < oStorageConnections.Size(); ++i )
    {
        bool bFoundKey = false;

        for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;
            if(!current)
                continue;

            if(IsObjectExist(current, oStorageConnections[i]))
            {
                bFoundKey = true;
                mnIds[current->GetId()] = true;
                break;
            }
        }

        if(!bFoundKey)
        {
            wxGxObject* pNewGxObject = CreateChildGxObject(oStorageConnections[i]);
            //ObjectAdded event
            if(pGxCatalog)
                pGxCatalog->ObjectAdded(pNewGxObject->GetId());
            mnIds[pNewGxObject->GetId()] = true;
        }
	}

    if(mnIds.empty())//delete all items
    {
        if(GetChildren().GetCount() != 0)
        {
            DestroyChildren();
            if(pGxCatalog)
                pGxCatalog->ObjectChanged(GetId());
        }
    }
    else
    {
        //delete items
        for(std::map<long, bool>::const_iterator i = mnIds.begin(); i != mnIds.end(); ++i)
        {
            if(pGxCatalog && i->second == false)
            {
                if(DestroyChild(pGxCatalog->GetRegisterObject(i->first)))
                    pGxCatalog->ObjectDeleted(i->first);
            }
        }
    }
}

bool wxGxJSONConnectionStorage::DeleteItemById(int nStoreId)
{
    if(!wxFileName::FileExists(m_sStoragePath))
    {
        return false;
    }

    wxFileInputStream StorageInputStream(m_sStoragePath);
    wxJSONReader oStorageReader;
    wxJSONValue  oStorageRoot;
    //try to load connections storage file
    int numErrorsStorage = oStorageReader.Parse( StorageInputStream, &oStorageRoot );
    if ( numErrorsStorage > 0 )  {
        const wxArrayString& errors = oStorageReader.GetErrors();
        wxString sErrMsg(_("The JSON Storage document is not well-formed"));
        for (size_t i = 0; i < errors.GetCount(); ++i)
        {
            wxString sErr = errors[i];
            sErrMsg.Append(wxT("\n"));
            sErrMsg.Append(wxString::Format(wxT("%d. %s"), i, sErr.c_str()));
        }
        wxLogError(sErrMsg);
        return false;
    }

    wxJSONValue& oStorageConnections = oStorageRoot[wxT("connections")];
    for( int i = 0; i < oStorageConnections.Size(); ++i )
    {
        if(oStorageConnections[i][wxT("id")].AsInt() == nStoreId)
        {
            if(oStorageConnections.Remove(i))
            {
                i--;
            }
        }
    }

    wxJSONWriter writer( wxJSONWRITER_STYLED | wxJSONWRITER_WRITE_COMMENTS );    
    wxString  sJSONText;

    writer.Write( oStorageRoot, sJSONText );
    wxFile oStorageFile(m_sStoragePath, wxFile::write);
    if(!oStorageFile.Write(sJSONText))
    {
        oStorageFile.Close();
        return false;
    }
    
    return oStorageFile.Close();
}


bool wxGxJSONConnectionStorage::RenameItemById(int nStoreId, const wxString& NewName)
{
    if(!wxFileName::FileExists(m_sStoragePath))
    {
        return false;
    }

    wxFileInputStream StorageInputStream(m_sStoragePath);
    wxJSONReader oStorageReader;
    wxJSONValue  oStorageRoot;
    //try to load connections storage file
    int numErrorsStorage = oStorageReader.Parse( StorageInputStream, &oStorageRoot );
    if ( numErrorsStorage > 0 )  {
        const wxArrayString& errors = oStorageReader.GetErrors();
        wxString sErrMsg(_("The JSON Storage document is not well-formed"));
        for (size_t i = 0; i < errors.GetCount(); ++i)
        {
            wxString sErr = errors[i];
            sErrMsg.Append(wxT("\n"));
            sErrMsg.Append(wxString::Format(wxT("%d. %s"), i, sErr.c_str()));
        }
        wxLogError(sErrMsg);
        return false;
    }

    wxJSONValue& oStorageConnections = oStorageRoot[wxT("connections")];
    for( int i = 0; i < oStorageConnections.Size(); ++i )
    {
        if(oStorageConnections[i][wxT("id")].AsInt() == nStoreId)
        {
            oStorageConnections[i][wxT("name")] = NewName;
            break;
        }
    }

    wxJSONWriter writer( wxJSONWRITER_STYLED | wxJSONWRITER_WRITE_COMMENTS );    
    wxString  sJSONText;

    writer.Write( oStorageRoot, sJSONText );
    wxFile oStorageFile(m_sStoragePath, wxFile::write);
    if(!oStorageFile.Write(sJSONText))
    {
        oStorageFile.Close();
        return false;
    }
    
    return oStorageFile.Close();
}

bool wxGxJSONConnectionStorage::AddItem(int nStoreId, const wxString& sName, const wxString& sPath)
{
    if(!wxFileName::FileExists(m_sStoragePath) || sPath.IsEmpty())
    {
        return false;
    }

    //add
    wxFileInputStream StorageInputStream(m_sStoragePath);
    wxJSONReader oStorageReader;
    wxJSONValue  oStorageRoot;
    //try to load connections json file
    int numErrorsStorage = oStorageReader.Parse( StorageInputStream, &oStorageRoot );
    if ( numErrorsStorage > 0 )  {
        const wxArrayString& errors = oStorageReader.GetErrors();
        wxString sErrMsg(_("The JSON Storage document is not well-formed"));
        for (size_t i = 0; i < errors.GetCount(); ++i)
        {
            wxString sErr = errors[i];
            sErrMsg.Append(wxT("\n"));
            sErrMsg.Append(wxString::Format(wxT("%d. %s"), i, sErr.c_str()));
        }
        wxLogError(sErrMsg);
        return false;
    }

    //try to load connections storage file
    wxJSONValue& oStorageConnections = oStorageRoot[wxT("connections")];
    int nNewItem = oStorageConnections.Size();
    oStorageConnections[nNewItem][wxT("name")] = sName;
    oStorageConnections[nNewItem][wxT("path")] = sPath;
    oStorageConnections[nNewItem][wxT("id")] = nStoreId;

    wxJSONWriter writer( wxJSONWRITER_STYLED | wxJSONWRITER_WRITE_COMMENTS );    
    wxString  sJSONText;

    writer.Write( oStorageRoot, sJSONText );
    wxFile oStorageFile(m_sStoragePath, wxFile::write);
    if(!oStorageFile.Write(sJSONText))
    {
        oStorageFile.Close();
        return false;
    }
    
    return oStorageFile.Close();
}