/******************************************************************************
 * Project:  wxGIS
 * Purpose:  DropTarget implementations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Bishop
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
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnections.h"

#ifdef wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// wxGISDropTarget
// ----------------------------------------------------------------------------

wxGISDropTarget::wxGISDropTarget(IViewDropTarget *pOwner)
{
    m_pOwner = pOwner;
    wxDataObjectComposite* dataobj = new wxDataObjectComposite();
    dataobj->Add(new wxGISStringDataObject(wxDataFormat(wxT("application/x-vnd.wxgis.gxobject-name"))), true);
    dataobj->Add(new wxFileDataObject());
    //dataobj->Add(new wxGISDecimalDataObject(wxDataFormat(wxT("application/x-vnd.wxgis.gxobject-id"))));
    //dataobj->Add(new wxDataObjectSimple(wxDataFormat(wxT("application/x-vnd.qgis.qgis.uri"))));
    SetDataObject(dataobj);
}

wxGISDropTarget::~wxGISDropTarget(void)
{
}

wxDragResult wxGISDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    return m_pOwner->OnDragOver(x, y, def);
}

wxDragResult wxGISDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    return def;
}

void wxGISDropTarget::OnLeave()
{
    m_pOwner->OnLeave();
}

bool wxGISDropTarget::OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects)
{
    return m_pOwner->OnDropObjects(x, y, GxObjects);
}

wxDragResult wxGISDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult)
{
    if ( !GetData() )
        return wxDragNone;

    //wxDragResult dragResult = wxDropTarget::OnData(x, y, defaultDragResult);
    //if ( dragResult == defaultDragResult )
    //{
        wxDataObjectComposite * dataobjComp = static_cast<wxDataObjectComposite *>(GetDataObject());

        wxDataFormat format = dataobjComp->GetReceivedFormat();
        wxDataObject *dataobj = dataobjComp->GetObject(format);

        //TODO:
        //application/x-vnd.wxgis.gxobject-id
        //application/x-vnd.qgis.qgis.uri

        wxDataFormat PrefferedDataFormat(wxT("application/x-vnd.wxgis.gxobject-name"));
        if(format == PrefferedDataFormat)
        {
            wxGISStringDataObject* pStringDataObject = static_cast<wxGISStringDataObject *>(dataobj);
            if(pStringDataObject)
                return OnDropObjects( x, y, pStringDataObject->GetStrings() ) ? defaultDragResult : wxDragNone;
        }

        if(format.GetType() == wxDF_FILENAME)
        {
            //Convert from file path to gxname
            wxFileDataObject* pFileDataObject = static_cast<wxFileDataObject *>(dataobj);
            if(pFileDataObject)
            {
                wxArrayString asSystemPaths = pFileDataObject->GetFilenames();
                wxArrayString asObjects = PathsToNames(asSystemPaths);
                if(asObjects.GetCount() > 0)
                    return OnDropObjects( x, y, asObjects ) ? defaultDragResult : wxDragNone;
            }
        }

        wxFAIL_MSG( "unexpected data object format" );

    //}

    return defaultDragResult;
}

wxArrayString wxGISDropTarget::PathsToNames(const wxArrayString saPaths)
{
    wxArrayString asObjects;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    wxGxObjectContainer* pConnections = wxDynamicCast(pGxCatalog->GetRootItemByType(wxCLASSINFO(wxGxDiscConnections)), wxGxObjectContainer);
    if(pConnections)
    {
        wxGxObjectList::const_iterator iter;  
        for(size_t i = 0; i < saPaths.GetCount(); ++i)
        {
            for(iter = pConnections->GetChildren().begin(); iter != pConnections->GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                wxGxObject *searched = current->FindGxObjectByPath(saPaths[i]);
                if(searched)
                {
                    asObjects.Add(searched->GetFullName());
                    break;
                }
            }
        }
    }
    return asObjects;
}

#endif // wxUSE_DRAG_AND_DROP

    /*
    GxObjectArray Array;
    bool bValid(false);
    //1. try to get initiated GxObjects
	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
	if(pGxObjectContainer)
    {
        for(size_t i = 0; i < filenames.GetCount(); ++i)
        {
            IGxObject* pGxObj(NULL);
            //as the names is internal check the /vsi
            wxString sCatalogNameProb;
            if(filenames[i].StartsWith(wxT("/vsi"), &sCatalogNameProb))
            {
                int nSlashPos = sCatalogNameProb.Find(wxT("/"));
                if(nSlashPos != wxNOT_FOUND)
                {
                    sCatalogNameProb = sCatalogNameProb.Right(sCatalogNameProb.Len() - nSlashPos - 1);
                    if(wxFileName::GetPathSeparator() == '/')
                        sCatalogNameProb.Replace(wxT("\\"), wxFileName::GetPathSeparator());
                    else
                        sCatalogNameProb.Replace(wxT("/"), wxFileName::GetPathSeparator());
                    pGxObj = pGxObjectContainer->SearchChild(sCatalogNameProb);
                }
            }
            else
            {
                pGxObj = pGxObjectContainer->SearchChild(filenames[i]);
            }
            if(pGxObj)
            {
                bValid = true;
                Array.push_back(pGxObj);
            }
        }
    }

    if(Array.empty())
    {
        char **papszFileList = NULL;
        CPLString szPath;
        CPLString szPathParent;
        IGxObject* pGxParentObj(NULL);

        for(size_t i = 0; i < filenames.GetCount(); ++i)
        {
            //Change to CPLString
            CPLString szFilePath(filenames[i].mb_str(wxConvUTF8));
            if(i == 0)
            {
                szPath = CPLGetPath(szFilePath);
                //drop /vsi
                if(szPath[0] == '/')
                {
                    for(size_t j = 1; j < szPath.length(); ++j)
                    {
                        if(szPath[j] == '/')
                        {
                            szPathParent = CPLString(&szPath[j + 1]);
                            break;
                        }
                    }
                }
                else
                    szPathParent = szPath;
                //get GxObj
            }
            papszFileList = CSLAddString( papszFileList, szFilePath );
        }

        if(!m_pCatalog->GetChildren(szPath, papszFileList, Array))
        {
            CSLDestroy( papszFileList );
            return false;
        }
        else
        {
            //Attach - cat & parent
            if(!szPathParent.empty())
            {
                pGxParentObj = pGxObjectContainer->SearchChild(wxString(szPathParent, wxConvUTF8));
                if(pGxParentObj)
                {
                    bValid = true;
                    for(size_t i = 0; i < Array.size(); ++i)
                        Array[i]->Attach(pGxParentObj, m_pCatalog);
                }
            }
        }
        CSLDestroy( papszFileList );
    }

    if(!bValid)
        return false;
    //create progress dialog
    wxString sTitle = wxString::Format(_("%s %d objects (files)"), bMove == true ? _("Move") : _("Copy"), filenames.GetCount());
    wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());
    wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), 100, pParentWnd, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

    for(size_t i = 0; i < Array.size(); ++i)
    {
		wxString sMessage = wxString::Format(_("%s %d object (file) from %d"), bMove == true ? _("Move") : _("Copy"), i + 1, Array.size());
		ProgressDlg.SetTitle(sMessage);
		ProgressDlg.PutMessage(sMessage);
        if(!ProgressDlg.Continue())
            break;

        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(Array[i]);
        if(pGxObjectEdit)
        {
            //IGxObjectContainer* pGxParentObjectContainer = dynamic_cast<IGxObjectContainer*>(Array[i]->GetParent());
            if(bMove && pGxObjectEdit->CanMove(m_sPath))
            {
                if(!pGxObjectEdit->Move(m_sPath, &ProgressDlg))
                {
                    //pGxParentObjectContainer->DeleteChild(Array[i]);

                    //bool ret_code = AddChild(Array[i]);
                    //if(!ret_code)
                    //{
                    //    wxDELETE(Array[i]);
                    //}
                    //else
                    //{
                    //    m_pCatalog->ObjectAdded(Array[i]->GetID());
                    //    m_pCatalog->ObjectRefreshed(pGxParentObj->GetID());
                    //}
                    return false;
                }
            }
            else if(!bMove && pGxObjectEdit->CanCopy(m_sPath))
            {
                if(!pGxObjectEdit->Copy(m_sPath, &ProgressDlg))
                {
                    //bool ret_code = AddChild(Array[i]);
                    //if(!ret_code)
                    //{
                    //    wxDELETE(Array[i]);
                    //}
                    //else
                    //{
                    //    m_pCatalog->ObjectAdded(Array[i]->GetID());
                    //}
                    return false;
                }
            }
            else
                return false;
        }
    }
    m_bIsChildrenLoaded = false;
    LoadChildren();
    m_pCatalog->ObjectRefreshed(GetId());

	ProgressDlg.SetValue(ProgressDlg.GetValue() + 1);
	ProgressDlg.Destroy();*/