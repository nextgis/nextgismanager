/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGIS Networking header. Network classes for remote server
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2013 Dmitry Baryshnikov
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

#include "wx/socket.h"

#if wxUSE_IPV6
    typedef wxIPV6address IPaddress;
#else
    typedef wxIPV4address IPaddress;
#endif

/** \enum wxGISMessagePriority net.h
 *  \brief The message priority type.
 */
enum wxGISMessagePriority
{
	enumGISPriorityLowest   = 0, 
	enumGISPriorityLow      = 25,
	enumGISPriorityNormal   = 50,
	enumGISPriorityHigh     = 75,
    enumGISPriorityHighest  = 100
};

/** \enum wxGISNetCommand
    \brief A network command types.

    This is predefined command types to network message.
*/
enum wxGISNetCommand
{
    enumGISNetCmdUnk = 0,/**< The type is unknown */ 
    enumGISNetCmdHello,  /**< The connect to server command - usually with login and pass */ 
    enumGISNetCmdCmd,    /**< The network message is a command */ 
    enumGISNetCmdNote,   /**< The network message is a notification */ 
    enumGISNetCmdBye,    /**< The network message is a disconnect notification */ 
    enumGISNetCmdSetExitSt  /**< The network message is a exit state for server*/
};

/** \enum wxGISNetCommand
    \brief A network command description types.

    This is command description types to network message.
*/
enum wxGISNetCommandState
{    
    enumGISNetCmdStUnk = 0,	/**< The type is unknown */
    //status
    enumGISNetCmdStOk,      /**< The indicator for any net command - OK */
	enumGISNetCmdStErr,     /**< The indicator for any net command - Error */
	enumGISNetCmdStTimeout,     /**< The indicator for any net command - Timeout */
    enumGISNetCmdStAccept,  /**< The indicator for enumGISNetCmdHello net command - Connection accepted */
    enumGISNetCmdStRefuse,  /**< The indicator for enumGISNetCmdHello net command - Connection refused */
    //command
    enumGISCmdChildren,  /**< The indicator for enumGISNetCmdCmd net command - Get object children */
    enumGISCmdDetails,   /**< The indicator for enumGISNetCmdCmd net command - Get object details */
    enumGISCmdStAdd,        /**< The indicator for enumGISNetCmdCmd net command - Add new object */
    enumGISCmdStDel,        /**< The indicator for enumGISNetCmdCmd net command - Delete object */
    enumGISCmdStChng,       /**< The indicator for enumGISNetCmdCmd net command - Change object */  
    enumGISCmdSetParam,      /**< The indicator for enumGISNetCmdCmd net command - Set parameters */
    enumGISCmdGetParam,      /**< The indicator for enumGISNetCmdCmd net command - Get parameters */
    //exit state
    enumGISNetCmdStExit,
    enumGISNetCmdStNoExit,
    enumGISNetCmdStExitAfterExec,
    //task
    enumGISCmdStStart,      /**< The indicator for enumGISNetCmdCmd net command - Start task execution - change state? */
    enumGISCmdStStop,       /**< The indicator for enumGISNetCmdCmd net command - Stop task execution - change state? */    
    enumGISCmdStPriority,   /**< The indicator for enumGISNetCmdCmd net command - Set task priority - change state? */    
    enumGISCmdNoteMsg,      /**< The indicator for enumGISNetCmdNote net notification - New message from task*/   
    //enumGISCmdNoteVol,      /**< The indicator for enumGISNetCmdNote net notification - Task volume changed*/ 
    //enumGISCmdNotePercent,  /**< The indicator for enumGISNetCmdNote net notification - Task percent changed*/ 
    //other
    enumGISNetCmdStAuth     /**< The indicator for enumGISNetCmdHello net command - connect server */
        /*/notifications
    ,
    enumGISMsgStSnd,
    enumGISMsgStRcv*/
};

