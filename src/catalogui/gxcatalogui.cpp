/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalogUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxpending.h"
#include "wxgis/framework/progressdlg.h"
#include "wxgis/framework/applicationbase.h"

#include "../../art/mainframecat.xpm"
#include "../../art/process_working_16.xpm"
#include "../../art/process_working_48.xpm"

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxCatalogUI, wxGxCatalog);

wxGxCatalogUI::wxGxCatalogUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxCatalog(oParent, soName, soPath)
{
    m_oIcon = wxIcon(mainframecat_xpm);
	
    if(m_oaPendingIconsSmall.empty())
    {
		wxBitmap lst(process_working_16_xpm);
		for(size_t i = 0; i < lst.GetWidth(); i += 16)
		{
			wxIcon ico;
			ico.CopyFromBitmap(lst.GetSubBitmap(wxRect(i, 0, 16, 16)));
			m_oaPendingIconsSmall.push_back(ico);
		}
    }
	
    if(m_oaPendingIconsLarge.empty())
    {
		wxBitmap lst(process_working_48_xpm);
		for(size_t i = 0; i < lst.GetWidth(); i += 48)
		{
			wxIcon ico;
			ico.CopyFromBitmap(lst.GetSubBitmap(wxRect(i, 0, 48, 48)));
			m_oaPendingIconsLarge.push_back(ico);
		}
    }
}

wxGxCatalogUI::~wxGxCatalogUI(void)
{
}

wxIcon wxGxCatalogUI::GetLargeImage(void)
{
    return wxNullIcon;
}

wxIcon wxGxCatalogUI::GetSmallImage(void)
{
    return m_oIcon;
}

long wxGxCatalogUI::AddPending(long nParentId)
{
	wxCriticalSectionLocker lock(m_PendCriticalSect);
    wxGxObjectContainer *pGxObjectContainer = wxDynamicCast(GetRegisterObject(nParentId), wxGxObjectContainer);
    wxCHECK_MSG(pGxObjectContainer, wxNOT_FOUND, wxT("The parent GxObject is not exist or not a container"));

    wxGxPendingUI *pPend = new wxGxPendingUI(&m_oaPendingIconsSmall, &m_oaPendingIconsLarge, pGxObjectContainer);
    return pPend->GetId();
}

void wxGxCatalogUI::RemovePending(long nPendingId)
{
 	wxCriticalSectionLocker lock(m_PendCriticalSect);
    if(nPendingId == wxNOT_FOUND)
        return;
    wxGxPendingUI *pPend = wxDynamicCast(GetRegisterObject(nPendingId), wxGxPendingUI);
    wxCHECK_RET(pPend, wxT("The Pending GxObject is not exist"));
    pPend->StopAndDestroy();
	
	//RemoveChild
}

//------------------------------------------------------------------------------------------
// separate ops
//------------------------------------------------------------------------------------------
bool FolderDrop(const CPLString& pPath, const wxArrayString& GxObjectPaths, bool bMove)
{
    if(GxObjectPaths.GetCount() == 0)
        return false;

    //TODO: check input object type and output object type
    //if different types - export
    //dialog with wxGIS and choices for output types (raster and vector data)

    //create progress dialog
    wxString sOper(bMove == true ? _("Move") : _("Copy"));
    wxString sTitle = wxString::Format(_("%s %ld objects (files)"), sOper.c_str(), GxObjectPaths.GetCount());
    wxWindow* pParentWnd = dynamic_cast<wxWindow*>(GetApplication());

    wxGISProgressDlg ProgressDlg(sTitle, _("Begin operation..."), GxObjectPaths.GetCount(), pParentWnd);
    ProgressDlg.ShowProgress(true);
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    bool bCopyAsk = true;

    wxWindowDisabler dis(&ProgressDlg);
    for(size_t i = 0; i < GxObjectPaths.GetCount(); ++i)
    {
		wxString sMessage = wxString::Format(_("%s %ld object (file) from %ld"), sOper.c_str(), i + 1, GxObjectPaths.GetCount());
//		ProgressDlg.SetTitle(sMessage);
		ProgressDlg.PutMessage(sMessage);
        if(!ProgressDlg.Continue())
            break;		
		
        ProgressDlg.SetValue(i);

        wxString sPath = GxObjectPaths[i];
        wxGxObject* pGxObj = pCatalog->FindGxObject(sPath);
        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(pGxObj);
        if(pGxObjectEdit)
        {
            if(bMove && pGxObjectEdit->CanMove(pPath))
            {
                bool bShouldMove = true;
                wxGxObject* pParentGxObj = pGxObj->GetParent();
                if (pParentGxObj != NULL)
                {
                    bShouldMove = pParentGxObj->GetPath() != pPath;
                }
                if (bShouldMove)
                {
                    if(!pGxObjectEdit->Move(pPath, &ProgressDlg))
                    {
                        wxGISErrorMessageBox(wxString::Format(_("%s failed. Path: %s"), _("Move"), pGxObj->GetFullName()));
                        return false;
                    }
					continue;
                }
            }
            
			if(pGxObjectEdit->CanCopy(pPath))
            {
                if (bMove && bCopyAsk)
                {
                    bCopyAsk = false;
                    if (wxMessageBox(wxString::Format(_("Cannot move path: %s.\nBut can copy. Proceed?"), pGxObj->GetFullName()), _("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxNO)
                        return false;

                }
                if(!pGxObjectEdit->Copy(pPath, &ProgressDlg))
                {
                    wxGISErrorMessageBox(wxString::Format(_("%s failed. Path: %s"), _("Copy"), pGxObj->GetFullName()));
                    return false;
                }
				
				continue;
            }
			
            return false;
        }
    }

    return true;
}

bool wxGxCatalogUI::Init(void)
{
	if(m_bIsInitialized)
		return true;

	if(!wxGxCatalog::Init())
        return false;

    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

	m_bOpenLastPath = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/open_last_path")), false);


    m_bIsInitialized = true;
    return true;
}
