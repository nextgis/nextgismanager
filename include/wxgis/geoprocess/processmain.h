/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2011 Bishop
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
#pragma once

#include "wxgis/core/init.h"
#include "wxgis/geoprocess/geoprocess.h"

#include <wx/string.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/ffile.h>

bool parse_commandline_parameters( wxCmdLineParser& parser );
int main(int argc, char** argv);

/** \class wxGPTaskExecutor processmain.h
 *  \brief The geoprocessing task exec class.
 */

class wxGPTaskExecutor : 
    public ITrackCancel,
	public IProgressor,
	public wxGISInitializer
{
public:
    wxGPTaskExecutor(void);
    virtual ~wxGPTaskExecutor(void);
    // wxGPTaskExecutor
    virtual bool OnExecute(wxString sToolName, wxString sToolParameters);
    // ITrackCancel
	virtual void PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType);
    // IProgressor
    virtual void SetValue(int value);
    virtual int GetValue(void){return m_nValue;};
    virtual bool Show(bool bShow){return true;};
    virtual void SetRange(int range){m_nRange = range;};
    virtual int GetRange(void){return m_nRange;};
    virtual void Play(void){};
    virtual void Stop(void){};
	virtual void SetYield(bool bYield = false){};
	//wxGISInitializer
	virtual bool Initialize(const wxString &sAppName, const wxString &sLogFilePrefix, wxCmdLineParser& parser);
	virtual void Uninitialize();
protected:
    int m_nValue;
    int m_nRange;
    wxTextOutputStream* m_pOutTxtStream;
    wxFFile m_StdOutFile;
    wxString m_sPrevMsg;
};
