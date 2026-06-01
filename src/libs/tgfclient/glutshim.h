/***************************************************************************
    glutshim.h -- Route GLUT includes to FreeGLUT or SDL2 shim
***************************************************************************/

#ifndef __GLUTSHIM_H__
#define __GLUTSHIM_H__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef TORCS_USE_SDL2
# include "sdl2glut.h"
#else
# include <GL/glut.h>
#endif

#endif /* __GLUTSHIM_H__ */
