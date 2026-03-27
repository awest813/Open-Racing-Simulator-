/***************************************************************************

    file        : careermenu.h
    created     : 2024
    copyright   : (C) 2024 Open Racing Simulator Contributors
    version     : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file
    Career mode menu entry point.
*/

#ifndef _CAREERMENU_H_
#define _CAREERMENU_H_

/** Initialize and return the career mode selection screen.
 *  Shows options to start a new career or continue an existing one.
 *  @param prevMenu  Screen handle to return to when the player presses Back.
 *  @return Screen handle for the career selection screen.
 */
extern void *CareerMenuInit(void *prevMenu);

#endif /* _CAREERMENU_H_ */
