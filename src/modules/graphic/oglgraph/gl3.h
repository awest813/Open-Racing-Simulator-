/***************************************************************************
 *
 *   file                 : gl3.h
 *   purpose              : OpenGL 3.3 Core Profile function declarations
 *                          and cross-platform loader shim.
 *
 *   On Linux / macOS with a modern GL driver, GL_GLEXT_PROTOTYPES makes
 *   all function symbols available from the dynamic linker directly.
 *
 *   On Windows, OpenGL functions beyond 1.1 are NOT exported from
 *   opengl32.dll by name — they must be fetched via wglGetProcAddress.
 *   We declare `extern` function pointer variables here and provide a
 *   separate gl_loader.cpp that resolves them at runtime.
 *
 ***************************************************************************/

#pragma once

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <GL/gl.h>   // GL 1.1 from system

/* -----------------------------------------------------------------------
 * Bring in OpenGL type definitions (GLenum, GLuint, etc.) that are needed
 * for the declarations below. We use the bundled glext.h for the typedefs.
 * -----------------------------------------------------------------------*/
#  ifndef APIENTRY
#    define APIENTRY __stdcall
#  endif
#  ifndef GLAPI
#    define GLAPI extern
#  endif

/* Fundamental types introduced in newer GL */
typedef char          GLchar;
typedef ptrdiff_t     GLsizeiptr;
typedef ptrdiff_t     GLintptr;

// ---- Shader objects -------------------------------------------------------
typedef unsigned int  GLuint;   // already in gl.h but safe to re-typedef

/* Declare function pointer types for GL 3.3 functions */
#define GL3FUNC(ret, name, ...) typedef ret (APIENTRY * PFN##name##PROC)(__VA_ARGS__); extern PFN##name##PROC name;

// Shaders
GL3FUNC(GLuint, glCreateShader, GLenum)
GL3FUNC(void,   glShaderSource, GLuint, GLsizei, const GLchar* const*, const GLint*)
GL3FUNC(void,   glCompileShader, GLuint)
GL3FUNC(void,   glGetShaderiv, GLuint, GLenum, GLint*)
GL3FUNC(void,   glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar*)
GL3FUNC(void,   glDeleteShader, GLuint)
GL3FUNC(GLuint, glCreateProgram,)
GL3FUNC(void,   glAttachShader, GLuint, GLuint)
GL3FUNC(void,   glDetachShader, GLuint, GLuint)
GL3FUNC(void,   glLinkProgram, GLuint)
GL3FUNC(void,   glGetProgramiv, GLuint, GLenum, GLint*)
GL3FUNC(void,   glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar*)
GL3FUNC(void,   glDeleteProgram, GLuint)
GL3FUNC(void,   glUseProgram, GLuint)
GL3FUNC(GLint,  glGetUniformLocation, GLuint, const GLchar*)
GL3FUNC(void,   glUniform1i, GLint, GLint)
GL3FUNC(void,   glUniform1f, GLint, GLfloat)
GL3FUNC(void,   glUniform2f, GLint, GLfloat, GLfloat)
GL3FUNC(void,   glUniform3f, GLint, GLfloat, GLfloat, GLfloat)
GL3FUNC(void,   glUniform4f, GLint, GLfloat, GLfloat, GLfloat, GLfloat)
GL3FUNC(void,   glUniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat*)

// Buffers / VAO
GL3FUNC(void,   glGenVertexArrays, GLsizei, GLuint*)
GL3FUNC(void,   glBindVertexArray, GLuint)
GL3FUNC(void,   glDeleteVertexArrays, GLsizei, const GLuint*)
GL3FUNC(void,   glGenBuffers, GLsizei, GLuint*)
GL3FUNC(void,   glBindBuffer, GLenum, GLuint)
GL3FUNC(void,   glBufferData, GLenum, GLsizeiptr, const void*, GLenum)
GL3FUNC(void,   glBufferSubData, GLenum, GLintptr, GLsizeiptr, const void*)
GL3FUNC(void,   glDeleteBuffers, GLsizei, const GLuint*)
GL3FUNC(void,   glVertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const void*)
GL3FUNC(void,   glEnableVertexAttribArray, GLuint)
GL3FUNC(void,   glDisableVertexAttribArray, GLuint)

// Framebuffer objects
GL3FUNC(void,   glGenFramebuffers, GLsizei, GLuint*)
GL3FUNC(void,   glBindFramebuffer, GLenum, GLuint)
GL3FUNC(void,   glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint)
GL3FUNC(void,   glDeleteFramebuffers, GLsizei, const GLuint*)
GL3FUNC(GLenum, glCheckFramebufferStatus, GLenum)
GL3FUNC(void,   glGenRenderbuffers, GLsizei, GLuint*)
GL3FUNC(void,   glBindRenderbuffer, GLenum, GLuint)
GL3FUNC(void,   glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei)
GL3FUNC(void,   glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint)
GL3FUNC(void,   glDeleteRenderbuffers, GLsizei, const GLuint*)

// Textures (modern)
GL3FUNC(void,   glActiveTexture, GLenum, ...)
GL3FUNC(void,   glGenerateMipmap, GLenum)

// Draw buffers
GL3FUNC(void,   glDrawBuffers, GLsizei, const GLenum*)
GL3FUNC(void,   glDrawElementsInstanced, GLenum, GLsizei, GLenum, const void*, GLsizei)

#undef GL3FUNC

/* Call once after wglCreateContext to resolve all function pointers */
bool gl3LoadFunctions();

/* GL 3.3 enum values not in old gl.h */
#ifndef GL_VERTEX_SHADER
#  define GL_VERTEX_SHADER         0x8B31
#  define GL_GEOMETRY_SHADER       0x8DD9
#  define GL_FRAGMENT_SHADER       0x8B30
#  define GL_COMPILE_STATUS        0x8B81
#  define GL_LINK_STATUS           0x8B82
#  define GL_INFO_LOG_LENGTH       0x8B84
#  define GL_ARRAY_BUFFER          0x8892
#  define GL_ELEMENT_ARRAY_BUFFER  0x8893
#  define GL_STATIC_DRAW           0x88B4
#  define GL_DYNAMIC_DRAW          0x88B8
#  define GL_FRAMEBUFFER           0x8D40
#  define GL_READ_FRAMEBUFFER      0x8CA8
#  define GL_DRAW_FRAMEBUFFER      0x8CA9
#  define GL_COLOR_ATTACHMENT0     0x8CE0
#  define GL_COLOR_ATTACHMENT1     0x8CE1
#  define GL_DEPTH_ATTACHMENT      0x8D00
#  define GL_RENDERBUFFER          0x8D41
#  define GL_DEPTH_COMPONENT       0x1902
#  define GL_DEPTH_COMPONENT24     0x81A6
#  define GL_DEPTH_COMPONENT32F    0x8CAC
#  define GL_FRAMEBUFFER_COMPLETE  0x8CD5
#  define GL_TEXTURE0              0x84C0
#  define GL_TEXTURE1              0x84C1
#  define GL_TEXTURE2              0x84C2
#  define GL_TEXTURE3              0x84C3
#  define GL_TEXTURE4              0x84C4
#  define GL_TEXTURE5              0x84C5
#  define GL_TEXTURE6              0x84C6
#  define GL_TEXTURE7              0x84C7
#  define GL_TEXTURE8              0x84C8
#  define GL_CLAMP_TO_EDGE         0x812F
#  define GL_CLAMP_TO_BORDER       0x812D
#  define GL_TEXTURE_WRAP_S        0x2802
#  define GL_TEXTURE_WRAP_T        0x2803
#  define GL_TEXTURE_MIN_FILTER    0x2801
#  define GL_TEXTURE_MAG_FILTER    0x2800
#  define GL_LINEAR                0x2601
#  define GL_NEAREST               0x2600
#  define GL_RGB16F                0x881B
#  define GL_RGBA16F               0x881A
#  define GL_RGB32F                0x8815
#  define GL_DEPTH_COMPONENT       0x1902
#  define GL_TEXTURE_BORDER_COLOR  0x1004
#  define GL_PROGRAM_POINT_SIZE    0x8642
#endif


#else // Non-Windows (Linux / macOS)

#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glext.h>

// No-op on non-Windows: symbols are resolved at link time.
inline bool gl3LoadFunctions() { return true; }

#endif // _WIN32
