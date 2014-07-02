/******************************************************************************
 * Project:  wxGIS
 * Purpose:  base crypt functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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
#pragma once

#include "wxgis/core/core.h"

/** \fn bool Crypt(const wxString &sText, wxString &sCryptText)
 *  \brief Crypt input string and convert it to HEX.
 *  \param sText input text
 *  \param sCryptText output crypted text
 *  \return true if succeeded, false otherwise
 */
bool WXDLLIMPEXP_GIS_CORE Crypt(const wxString &sText, wxString &sCryptText);
/** \fn bool Decrypt(const wxString &sText, wxString &sDecryptText)
 *  \brief Decrypt crypted text.
 *  \param sText input crypted text
 *  \param sDecryptText output decrypted text
 *  \return true if succeeded, false otherwise
 */
bool WXDLLIMPEXP_GIS_CORE Decrypt(const wxString &sText, wxString &sDecryptText);
/** \fn wxString GetRandomKey(short nLen)
 *  \brief Create random text. E.g random password
 *  \param nLen Text length
 *  \return Random text
 */
wxString WXDLLIMPEXP_GIS_CORE GetRandomKey(short nLen);


