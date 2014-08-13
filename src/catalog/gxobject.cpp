/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObject.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxobject.h"

static wxGxCatalogBase *g_pGxCatalog( NULL );

extern WXDLLIMPEXP_GIS_CLT wxGxCatalogBase* const GetGxCatalog(void)
{
	return g_pGxCatalog;
}

extern WXDLLIMPEXP_GIS_CLT void SetGxCatalog(wxGxCatalogBase* pCat)
{
    if(g_pGxCatalog != NULL)
        g_pGxCatalog->Destroy();
	g_pGxCatalog = pCat;
}

//---------------------------------------------------------------------------
// wxGxCatalogBase
//---------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxGxCatalogBase, wxGxObjectContainer);

wxGxCatalogBase::wxGxCatalogBase(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_nGlobalId = 0;
    m_bIsInitialized = false;

    m_bShowHidden = false;
    m_bShowExt = true;

    RegisterObject(this);
}

wxGxCatalogBase::~wxGxCatalogBase(void)
{
}

wxString wxGxCatalogBase::ConstructFullName(const wxGxObject* pObject) const
{
    wxCHECK_MSG( pObject, wxEmptyString, wxT("Input object pointer is null") );

	wxString sParentPath;
    if(pObject->GetParent())
        sParentPath = pObject->GetParent()->GetFullName();
    wxString sName = pObject->GetName();

	if(sParentPath.IsEmpty())
		return sName;
	else
	{
		if(sParentPath.EndsWith(wxFileName::GetPathSeparator()))
			return sParentPath + sName;
		else
			return sParentPath + wxFileName::GetPathSeparator() + sName;
    }
}

void wxGxCatalogBase::RegisterObject(wxGxObject* pObj)
{
	pObj->SetId(m_nGlobalId);
	m_moGxObject[m_nGlobalId] = pObj;
	m_nGlobalId++;
}

void wxGxCatalogBase::UnRegisterObject(long nId)
{
    m_moGxObject[nId] = NULL;
}

wxGxObject* const wxGxCatalogBase::GetRegisterObject(long nId)
{
    if( nId == wxNOT_FOUND )
        return NULL;
    return m_moGxObject[nId];
}

bool wxGxCatalogBase::Init(void)
{
	if(m_bIsInitialized)
		return true;

    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

	m_bShowHidden = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), false);
	m_bShowExt = oConfig.ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), true);

    //load GxObject Factories
	LoadObjectFactories();
    //load Children
	LoadChildren();

    m_bIsInitialized = true;
    return true;
}

bool wxGxCatalogBase::Destroy(void)
{
    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), m_bShowHidden);
        oConfig.Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), m_bShowExt);
    }

    return wxGxObjectContainer::Destroy();
}

wxString wxGxCatalogBase::GetFullName(void) const
{
    return wxEmptyString;
}

wxString wxGxCatalogBase::GetCategory(void) const
{
    return wxString(wxT("Root"));
}

bool wxGxCatalogBase::GetShowHidden(void) const
{
    return m_bShowHidden;
}

bool wxGxCatalogBase::GetShowExt(void) const
{
    return m_bShowExt;
}

void wxGxCatalogBase::SetShowHidden(bool bShowHidden)
{
    m_bShowHidden = bShowHidden;
}

void wxGxCatalogBase::SetShowExt(bool bShowExt)
{
    m_bShowExt = bShowExt;
}

bool wxGxCatalogBase::AreChildrenViewable(void) const
{
    return true;
}

//---------------------------------------------------------------------------
// wxGxObject
//---------------------------------------------------------------------------
wxIMPLEMENT_ABSTRACT_CLASS(wxGxObject, wxEvtHandler);

wxGxObject::wxGxObject(void) : wxEvtHandler()
{
    m_oParent = NULL;
    m_nId = wxNOT_FOUND;
}


wxGxObject::wxGxObject(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxEvtHandler()
{
    Create(oParent, soName, soPath);
}

void wxGxObject::SetParent(wxGxObject *oParent)
{
    m_oParent = oParent;
}

wxString wxGxObject::GetName(void) const
{
    return m_sName;
}

CPLString wxGxObject::GetPath(void) const
{
    return m_sPath;
}

void wxGxObject::SetName(const wxString &soName)
{
    m_sName = soName;
}

void wxGxObject::SetPath(const CPLString &soPath)
{
    m_sPath = soPath;
}

wxString wxGxObject::GetCategory(void) const
{
    return wxEmptyString;
}

wxGxObject *wxGxObject::GetParent(void) const
{
    return m_oParent;
}

bool wxGxObject::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    m_sName = soName;
    m_sPath = soPath;

    if(GetGxCatalog())
        GetGxCatalog()->RegisterObject(this);

    if( oParent && oParent->IsKindOf(wxCLASSINFO(wxGxObjectContainer)) )
    {
        wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(oParent, wxGxObjectContainer);
        if(pGxObjectContainer)
            pGxObjectContainer->AddChild(this);
    }
    else
        m_oParent = oParent;

    //Problems with treectrl and AreChildrenViewable
    //if(GetGxCatalog())
    //    GetGxCatalog()->ObjectAdded(GetId());


    //m_nId = nId;
    return true;
}

wxGxObject::~wxGxObject(void)
{
    // notify the parent about this window destruction
    if( m_oParent && m_oParent->IsKindOf(wxCLASSINFO(wxGxObjectContainer)) )
    {
        wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(m_oParent, wxGxObjectContainer);
        if(pGxObjectContainer)
        {
            pGxObjectContainer->RemoveChild(this);
        }
    }
    if(GetGxCatalog() && this != GetGxCatalog())
    {
        GetGxCatalog()->UnRegisterObject(GetId());
    }
}

wxString wxGxObject::GetBaseName(void) const
{
    wxFileName FileName(GetName());
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxObject::Destroy(void)
{
    if(GetGxCatalog())
        GetGxCatalog()->ObjectDeleted(GetId());

    delete this;
    return true;
}

void wxGxObject::Refresh(void)
{
    if(GetGxCatalog())
        GetGxCatalog()->ObjectRefreshed(GetId());
}

wxString wxGxObject::GetFullName(void) const
{
    if(GetGxCatalog())
	    return GetGxCatalog()->ConstructFullName(this);
    else
        return wxEmptyString;
};


wxGxObject *wxGxObject::FindGxObject(const wxString &sPath)
{
    wxString sObjName = GetFullName();
	if(sObjName.IsSameAs(sPath, false))
		return (wxGxObject *)this;
    return NULL;
}

wxGxObject *wxGxObject::FindGxObjectByPath(const wxString &sPath)
{
    wxString sThisPath(m_sPath, wxConvUTF8);
    if(sThisPath.IsSameAs(sPath, false))
        return (wxGxObject *)this;
    return NULL;
}

//---------------------------------------------------------------------------
// wxGxObjectContainer
//---------------------------------------------------------------------------

#if wxUSE_STD_CONTAINERS

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxGxObjectList)

#else // !wxUSE_STD_CONTAINERS

void wxGxObjectListNode::DeleteData()
{
    delete (wxGxObject *)GetData();
}

#endif // wxUSE_STD_CONTAINERS/!wxUSE_STD_CONTAINERS

wxIMPLEMENT_ABSTRACT_CLASS(wxGxObjectContainer, wxGxObject);

wxGxObjectContainer::wxGxObjectContainer() : wxGxObject()
{
}

wxGxObjectContainer::wxGxObjectContainer(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObject(oParent, soName, soPath)
{
}

wxGxObjectContainer::~wxGxObjectContainer(void)
{
    m_Children.Clear();
}

void wxGxObjectContainer::AddChild( wxGxObject *child )
{
    wxCHECK_RET( child, wxT("can't add a NULL child") );

    // this should never happen and it will lead to a crash later if it does
    // because RemoveChild() will remove only one node from the children list
    // and the other(s) one(s) will be left with dangling pointers in them
    wxASSERT_MSG( !GetChildren().Find(child), wxT("AddChild() called twice") );

    GetChildren().Append(child);
    child->SetParent(this);

    //if(GetGxCatalog())
    //    GetGxCatalog()->ObjectAdded(child->GetId());
}

const wxGxObjectList& wxGxObjectContainer::GetChildren() const
{
    return m_Children;
}

bool wxGxObjectContainer::CanCreate(long nDataType, long DataSubtype)
{
    return false;
}

wxGxObjectList& wxGxObjectContainer::GetChildren()
{
    return m_Children;
}

void wxGxObjectContainer::RemoveChild( wxGxObject *child )
{
    wxCHECK_RET( child, wxT("can't remove a NULL child") );

    GetChildren().DeleteObject(child);
    child->SetParent(NULL);
}

bool wxGxObjectContainer::DestroyChild( wxGxObject *child )
{
    wxCHECK_MSG( child, false, wxT("can't destroy a NULL child") );
    GetChildren().DeleteObject(child);
    child->Destroy();
    return true;
}

bool wxGxObjectContainer::DestroyChildren()
{
    wxLogDebug(wxT("Destroy: %s"), m_sName.c_str());
    wxGxObjectList::compatibility_iterator node;
    for ( ;; )
    {
        // we iterate until the list becomes empty
        node = GetChildren().GetFirst();
        if ( !node )
            break;

        wxGxObject *child = node->GetData();
        child->Destroy();

        wxASSERT_MSG( !GetChildren().Find(child), wxT("child didn't remove itself using RemoveChild()") );
    }
    return true;
}

bool wxGxObjectContainer::HasChildren(void)
{
    return !m_Children.IsEmpty();
}

wxGxObject *wxGxObjectContainer::FindGxObject(const wxString &sPath)
{
    wxGxObject *ret = wxGxObject::FindGxObject(sPath);
    if(ret)
        return ret;

    wxString sObjName = GetFullName().MakeLower();
    bool bHavePart = sPath.Lower().Find(sObjName) != wxNOT_FOUND;
    if(bHavePart && HasChildren())
    {
        wxGxObjectList::const_iterator iter;
        for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;

            wxGxObject *ret = current->FindGxObject(sPath);
            if(ret)
                return ret;
        }
    }
    return NULL;
}


bool wxGxObjectContainer::IsNameExist(const wxString &sName) const
{
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if( current->GetName().IsSameAs(sName) )
            return true;
	}
	return false;
}

void wxGxObjectContainer::Refresh(void)
{
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        current->Refresh();
    }

    wxGxObject::Refresh();
}

bool wxGxObjectContainer::Destroy(void)
{
    if(DestroyChildren())
        return wxGxObject::Destroy();
    return false;
}

wxGxObject *wxGxObjectContainer::FindGxObjectByPath(const wxString &sPath)
{
    wxGxObject *ret = wxGxObject::FindGxObjectByPath(sPath);
    if(ret)
        return ret;

    wxString sThisPath(m_sPath, wxConvUTF8);
    bool bHavePart = sPath.Lower().Find(sThisPath.MakeLower()) != wxNOT_FOUND;
    if(bHavePart && HasChildren())
    {
        wxGxObjectList::const_iterator iter;
        for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;

            wxGxObject *ret = current->FindGxObjectByPath(sPath);
            if(ret)
                return ret;
        }
    }
    return NULL;
}
