/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
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
#include "wxgis/framework/progressdlg.h"

//------------------------------------------------------------------------------
// wxGISProgressDlg
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISProgressDlg, wxProgressDialog)

wxGISProgressDlg::wxGISProgressDlg( const wxString &title, const wxString &message, int  maximum, wxWindow *  parent, int style ) : wxProgressDialog(title, message, maximum, parent, style), ITrackCancel()
{
	m_sLastMessage = message;
	m_nValue = 0;
	m_pProgressor = this;
}

wxGISProgressDlg::~wxGISProgressDlg(void)
{
}

bool wxGISProgressDlg::ShowProgress(bool bShow)
{
	return wxProgressDialog::Show(bShow);
}

void wxGISProgressDlg::SetRange(int range)
{
	wxProgressDialog::SetRange(range/* + 1*/);
}

int wxGISProgressDlg::GetRange(void) const
{
	return wxProgressDialog::GetRange()/* - 1*/;
}

int wxGISProgressDlg::GetValue(void) const
{
	return wxProgressDialog::GetValue();
}

void wxGISProgressDlg::Play(void)
{
	m_bIsCanceled = !wxProgressDialog::Pulse(m_sLastMessage);
}

void wxGISProgressDlg::Stop(void)
{
	m_bIsCanceled = !wxProgressDialog::Update(m_nValue, m_sLastMessage);
}

void wxGISProgressDlg::SetValue(int value)
{
	m_nValue = value;
	m_bIsCanceled = !wxProgressDialog::Update(m_nValue, m_sLastMessage);
}

void wxGISProgressDlg::Cancel(void)
{
	m_bIsCanceled = true;
}

bool wxGISProgressDlg::Continue(void)
{
	return !wxProgressDialog::WasCancelled();
}

void wxGISProgressDlg::Reset(void)
{
	m_bIsCanceled = false;
	wxProgressDialog::Resume();
}

void wxGISProgressDlg::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
	m_sLastMessage = sMessage;
	m_bIsCanceled = !wxProgressDialog::Update(m_nValue, m_sLastMessage);
}
