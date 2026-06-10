/***************************************************************************
                          img.cpp -- Images manipulation
                             -------------------
    created              : Tue Aug 17 20:13:08 CEST 1999
    copyright            : (C) 1999-2014 by Eric Espie, Bernhard Wymann
    email                : torcs@free.fr
    version              : $Id$
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
    Image management API.

    Load and store png images.
    @author Bernhard Wymann, Eric Espie
    @version $Id$
*/

#ifdef WIN32
#include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image_write.h"

#include <tgfclient.h>
#include <cstdlib>
#include <cstdio>
#ifdef WIN32
#include <direct.h>
#endif

#include <portability.h>

/** Load an image from disk to a buffer in RGBA mode.
    @ingroup	img		
    @param	filename	name of the image to load
    @param	widthp		width of the read image
    @param	heightp		height of the read image
    @param	screen_gamma	gamma correction value (unused in stb_image backend)
    @return	Pointer on the buffer containing the image
		<br>nullptr Error
 */
unsigned char *
GfImgReadPng(const char *filename, int *widthp, int *heightp, float screen_gamma)
{
	int channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *image_ptr = stbi_load(filename, widthp, heightp, &channels, 4);
	if (!image_ptr) {
		GfTrace("Can't open/parse file %s\n", filename);
		return nullptr;
	}
	return image_ptr;
}


/** Write a buffer to a png image on disk.
    @ingroup	img
    @param	img		image data (RGB)
    @param	filename	filename of the png file
    @param	width		width of the image
    @param	height		height of the image
    @return	0 Ok
		<br>-1 Error
 */
int
GfImgWritePng(unsigned char *img, const char *filename, int width, int height)
{
	stbi_flip_vertically_on_write(1);
	int success = stbi_write_png(filename, width, height, 3, img, width * 3);
	return success ? 0 : -1;
}

/** Free the texture
    @ingroup	img
    @param	tex	texture to free
    @return	none
*/
void
GfImgFreeTex(GLuint tex)
{
	if (tex != 0) {
		glDeleteTextures(1, &tex);
	}
}

/** Read a png image into a texture.
    @ingroup	img
    @param	filename	file name of the image
    @return	None.
 */
GLuint
GfImgReadTex(char *filename)
{
	void *handle;
	float screen_gamma;
	GLbyte *tex;
	int w, h;
	GLuint retTex;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), GFSCR_CONF_FILE);
	handle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
	screen_gamma = (float)GfParmGetNum(handle, GFSCR_SECT_PROP, GFSCR_ATT_GAMMA, nullptr, 2.0);
	tex = (GLbyte*)GfImgReadPng(filename, &w, &h, screen_gamma);

	if (!tex) {
		GfParmReleaseHandle(handle);
		return 0;
	}

	glGenTextures(1, &retTex);
	glBindTexture(GL_TEXTURE_2D, retTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)(tex));

	free(tex);

	GfParmReleaseHandle(handle);
	return retTex;
}
