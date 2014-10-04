/******************************************************************************
 * Project:  wxGIS (GIS)
 * Purpose:  core header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012,2014 Dmitry Baryshnikov
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

#include "wxgis/base.h"

#define wgDELETE(p,func) if(p != NULL) {p->func; delete p; p = NULL;}
#define wsDELETE(p) if(p != NULL) {p->Release(); p = NULL;}
#define wsSET(mp, p) if(p != NULL) { mp = p; mp->Reference();}else{mp = NULL;}
#define wsGET(p) if(p != NULL){p->Reference(); return p;} else{return NULL;}
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define __YEAR__ ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
+ (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define VENDOR wxT("wxgis")

/** @enum wxGISEnumConfigKey

    The config key type.

    @library{core}
 */

enum wxGISEnumConfigKey
{
	enumGISHKLM = 0x0001,
	enumGISHKCU = 0x0002,
	//,
	//enumGISHKCC = 0x0004,
	//enumGISHKCR = 0x0008,
	enumGISHKAny = enumGISHKLM | enumGISHKCU
};

/** @enum wxGISEnumMessageType
    
    The process message type.

    @library{core}
 */

enum wxGISEnumMessageType
{
	enumGISMessageUnknown = 0,		/**< The message type is undefined */
	enumGISMessageError,        	/**< The error message*/
	enumGISMessageNormal,
	enumGISMessageQuestion,
    enumGISMessageInformation,  	/**< The information message*/
    enumGISMessageWarning,      	/**< The warning message*/
    enumGISMessageTitle,
    enumGISMessageOk,           	/**< The ok message - show ok icon near param edit control*/
    enumGISMessageSend,
    enumGISMessageReceive,
	enumGISMessageRequired,     	/**< The required message - show required icon near param edit control*/
	enumGISMessageNone         		/**< The none message - show no icon near param edit control*/
};

typedef struct _message
{ 
    wxGISEnumMessageType eType;
    wxString sMessage;
} MESSAGE;

/** @class IProgressor

    A progressor interface class. This is base class for progressors.

    @library{core}
*/
class IProgressor
{
public:
    /** \fn virtual ~IProgressor(void)
     *  \brief A destructor.
     */
	virtual ~IProgressor(void){};
	//pure virtual
    /** \fn bool ShowProgress(bool bShow)
     *  \brief Show/hide progressor.
     *  \param bShow The indicator to show (true) or hide (false) progressor
     *  \return The success of function execution
     */
    virtual bool ShowProgress(bool bShow) = 0;
    //
    /** \fn virtual void SetRange(int range)
     *  \brief Set progressor range.
     *  \param range The progressor value range
     */
    virtual void SetRange(int range) = 0;
    /** \fn int GetRange(void)
     *  \brief Set progressor range.
     *  \return The current progressor range
     */
    virtual int GetRange(void) const = 0;
    /** \fn void SetValue(int value)
     *  \brief Set progressor position.
     *  \param value The progressor current value
     */
    virtual void SetValue(int value) = 0;
    /** \fn int GetValue(void)
     *  \brief Get progressor position.
     *  \return The current progressor position
     */
    virtual int GetValue(void) const = 0;
    //
    /** \fn void Play(void)
     *  \brief Start undefined progressor state.
     */
    virtual void Play(void) = 0;
    /** \fn void Stop(void)
     *  \brief Stop undefined progressor state.
     */
    virtual void Stop(void) = 0;
    /** \fn void SetYield(bool bYield = false)
     *  \brief SetYield Yields control to pending messages in the windowing system.

	    This can be useful, for example, when a time-consuming process writes to a text window. Without an occasional yield, the text window will not be updated properly, and other processes will not respond.
     */
	virtual void SetYield(bool bYield = false) = 0;
};

/** \class ITrackCancel core.h
    \brief A TrackCancel interface class.

    This is base class for TrackCancel classes.
    The TrackCancel provide ability to stop by ESC, Cancel and etc. execution of some process or function
*/
class ITrackCancel
{
public:
    /** \fn ITrackCancel(void)
     *  \brief A constructor.
     */
    ITrackCancel(void) : m_bIsCanceled(false), m_pProgressor(NULL){};
    /** \fn virtual ~ITrackCancel(void)
     *  \brief A destructor.
     */
    virtual ~ITrackCancel(void){};
	virtual void Cancel(void)
    {
        m_bIsCanceled = true;
    };
	virtual bool Continue(void){return !m_bIsCanceled;};
	virtual void Reset(void){m_bIsCanceled = false;};
	virtual IProgressor* const GetProgressor(void){return m_pProgressor;};
	virtual void SetProgressor(IProgressor* pProgressor){m_pProgressor = pProgressor; };
	virtual void PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType){};
    virtual wxString GetLastMessage(void) const {return wxEmptyString;};
protected:
	bool m_bIsCanceled;
	IProgressor* m_pProgressor;
};

static bool CreateAndRunThread(wxThread* const pThread, wxString sClassName = wxEmptyString, const wxString sThreadName = wxEmptyString, int nPriority = WXTHREAD_DEFAULT_PRIORITY)
{
    wxCHECK_MSG(pThread, false, wxT("Input wxThread pointer is null"));

	if(sClassName.IsEmpty())
		sClassName = wxString(_("wxGISCore"));
    if ( pThread->Create() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't create %s Thread!"), sClassName.c_str(), sThreadName.c_str());
		return false;
    }
	pThread->SetPriority(nPriority);
	if(pThread->Run() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't run %s Thread!"), sClassName.c_str(), sThreadName.c_str());
		return false;
    }
	return true;
}

/** @enum wxGISEnumTaskStateType

    The process task state.

    @library{core}
 */
enum wxGISEnumTaskStateType
{
    enumGISTaskUnk = 0,
    enumGISTaskWork,
    enumGISTaskDone,
    enumGISTaskQuered,
    enumGISTaskPaused,
    enumGISTaskError
};

/** @enum wxGISEnumReturnType

    The function return state.

    @library{core}
 */
enum wxGISEnumReturnType
{
    enumGISReturnkUnk = 0,
    enumGISReturnOk,
    enumGISReturnFailed,
    enumGISReturnTimeout
};

//FLT_EPSILON DBL_EPSILON
inline bool IsDoubleEquil( double a, double b, double epsilon = EPSILON )
{
  const double diff = a - b;
  return diff > -epsilon && diff <= epsilon;
}

