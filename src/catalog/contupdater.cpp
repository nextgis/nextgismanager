/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/contupdater.h"
#include "wxgis/catalog/gxcatalog.h"

#define LONG_WAIT 12000
#define SHORT_WAIT 6500
#define SHORT_STEP 10

//------------------------------------------------------------------------------
// wxGxObjectContainerUpdater
//------------------------------------------------------------------------------
IMPLEMENT_ABSTRACT_CLASS(wxGxObjectContainerUpdater, wxGxObjectContainer)

wxGxObjectContainerUpdater::wxGxObjectContainerUpdater(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), wxThreadHelper(wxTHREAD_DETACHED)
{
	m_nLongWait = LONG_WAIT;
	m_nShortWait = SHORT_WAIT;
	
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
	{
        m_nLongWait = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/update_timeout/if_err_wait")), m_nLongWait);
		m_nShortWait = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/update_timeout/refresh_wait")), m_nShortWait);
	}
	
	m_nProcessUpdatesRequests = 0;
	m_bChildrenLoaded = false;
}

wxGxObjectContainerUpdater::~wxGxObjectContainerUpdater(void)
{

}

bool wxGxObjectContainerUpdater::CreateAndRunThread(void)
{
    if (GetThread() && GetThread()->IsRunning())
        return true;

    if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}

void wxGxObjectContainerUpdater::StopThread()
{
	if (GetThread() && GetThread()->IsRunning())
    {
        GetThread()->Delete();// Wait();
    }	
}

wxThread::ExitCode wxGxObjectContainerUpdater::Entry()
{
	int nShortStep = m_nShortWait / SHORT_STEP;
	
	while (!GetThread()->TestDestroy())
    {
        wxGxObjectMap smCurrentObjects = GetRemoteObjects();
        for (wxGxObjectMap::iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
		{
			wxGxObjectMap::iterator cit = smCurrentObjects.find(it->first);
			if (cit == smCurrentObjects.end())//delete
			{
				DeleteObject(it->first);
				m_smObjects.erase(it);
				if(m_smObjects.empty())
					break;
				it = m_smObjects.begin();
			}
			else if (cit->second != it->second)//rename
			{
				RenameObject(it->first, cit->second);
				it->second = cit->second;
			}
		}

		//add new
		for (wxGxObjectMap::iterator it = smCurrentObjects.begin(); it != smCurrentObjects.end(); ++it)
		{
			wxGxObjectMap::iterator cit = m_smObjects.find(it->first);
			if (cit == smCurrentObjects.end())
			{
				//refresh
				AddObject(it->first, it->second);				
				m_smObjects[it->first] = it->second;
			}
		}
			
		for ( size_t i = 0; i < SHORT_STEP; ++i ) 
		{    
			wxThread::Sleep(nShortStep); //min sleep is 450
			if(m_nProcessUpdatesRequests > 0)
			{
				m_nProcessUpdatesRequests--;
				break;
			}			
		}
	}

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxGxObjectContainerUpdater::DeleteObject(int nRemoteId)
{
	wxGxObject *pObj = GetChildByRemoteId(nRemoteId);
	if(NULL != pObj)
	{
		DestroyChild(pObj);
	}
}

void wxGxObjectContainerUpdater::RenameObject(int nRemoteId, const wxString &sNewName)
{
	wxGxObject *pObj = GetChildByRemoteId(nRemoteId);
	if(NULL != pObj)
	{
		pObj->SetName(sNewName);
		CPLString szNewSchemaName(pObj->GetName().ToUTF8());
		pObj->SetPath(CPLFormFilename(CPLGetPath(pObj->GetPath()), sNewName, NULL));
		wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, pObj->GetId());
	}
}

void wxGxObjectContainerUpdater::OnGetUpdates()
{
	m_nProcessUpdatesRequests++;
}

wxGxObject *wxGxObjectContainerUpdater::GetChildByRemoteId(int nRemoteId) const
{
	wxGxObjectList::const_iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); ++iter)
    {
        wxGxObject *current = *iter;
		wxGxRemoteId* pGxRemoteId = dynamic_cast<wxGxRemoteId*>(current);
        if (NULL != pGxRemoteId && pGxRemoteId->GetRemoteId() == nRemoteId)
			return current;
	}
	return NULL;
}

//------------------------------------------------------------------------------
// wxGxRemoteId
//------------------------------------------------------------------------------

wxGxRemoteId::wxGxRemoteId()
{
	
}

wxGxRemoteId::wxGxRemoteId(int nRemoteId)
{
	m_nRemoteId = nRemoteId;
}

wxGxRemoteId::~wxGxRemoteId()
{
	
}

int wxGxRemoteId::GetRemoteId() const 
{
	return m_nRemoteId;
}
