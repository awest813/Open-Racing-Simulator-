/***************************************************************************

    file        : portability.h
    created     : Fri Jul 8 15:19:34 CET 2005
    copyright   : (C) 2005 Bernhard Wymann
    email       : berniw@bluewin.ch
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

#ifndef _TORCS_PORTABILITY_H_
#define _TORCS_PORTABILITY_H_

#include <cstdlib>
#include <cstring>

#ifdef WIN32
#define HAVE_CONFIG_H
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Missing strndup, define it here (for FreeBSD).
// TODO: Move it into library.
// strndup code provided by Thierry Thomas.
#ifndef HAVE_STRNDUP

static char *strndup(const char *str, int len)
{
	char *ret;

	if ((str == nullptr || len < 0)) {
		return (nullptr);
	}

	ret = (char *) malloc(len + 1);
	if (ret == nullptr) {
		return (nullptr);
	}

	memcpy(ret, str, len);
	ret[len] = '\0';
	return (ret);
}

#endif

static const char* GfPathBaseName(const char* path)
{
	if (path == nullptr) {
		return "";
	}

	const char* forwardSlash = strrchr(path, '/');
	const char* backwardSlash = strrchr(path, '\\');
	const char* separator = forwardSlash;

	if (separator == nullptr || (backwardSlash != nullptr && backwardSlash > separator)) {
		separator = backwardSlash;
	}

	return (separator != nullptr) ? separator + 1 : path;
}


#ifdef WIN32
#if _MSC_VER < 1900 
#define snprintf _snprintf
#endif
#if _MSC_VER < 1500 
#define vsnprintf _vsnprintf
#endif
#endif

#ifdef WIN32
#include <cmath>

#if _MSC_VER < 1800
static float round(float x)
{
	return floor(x+0.5f);
}
#endif
#endif 

#endif // _TORCS_PORTABILITY_H_
