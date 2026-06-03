/***************************************************************************
 *
 *   file                 : gl_loader.cpp
 *   purpose              : Windows OpenGL 3.3 Core Profile function loader.
 *                          Defines all function pointer variables declared in
 *                          gl3.h and resolves them via wglGetProcAddress.
 *
 *   Call gl3LoadFunctions() once, immediately after the GL context is made
 *   current (i.e. after wglMakeCurrent).  The loader returns false and
 *   prints a diagnostic if any required function cannot be resolved.
 *
 ***************************************************************************/

#ifdef _WIN32

#include "gl3.h"
#include <tgf.h>    // GfOut / GfError

/* -----------------------------------------------------------------------
 * Define all function pointer variables (one per declaration in gl3.h)
 * -----------------------------------------------------------------------*/
#define GL3FUNC(ret, name, ...) PFN##name##PROC name = nullptr;

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

// Framebuffers
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

// Textures
GL3FUNC(void,   glActiveTexture, GLenum, ...)
GL3FUNC(void,   glGenerateMipmap, GLenum)

// Draw buffers
GL3FUNC(void,   glDrawBuffers, GLsizei, const GLenum*)
GL3FUNC(void,   glDrawElementsInstanced, GLenum, GLsizei, GLenum, const void*, GLsizei)

#undef GL3FUNC

/* -----------------------------------------------------------------------
 * Loader — fetches each function pointer via wglGetProcAddress
 * -----------------------------------------------------------------------*/

static void* loadProc(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    // wglGetProcAddress returns nullptr (or 1/2/3/-1) on failure.
    if (p == nullptr || p == (void*)1 || p == (void*)2 ||
        p == (void*)3 || p == (void*)-1)
    {
        // Fallback: try GetProcAddress on opengl32.dll for legacy symbols
        HMODULE gl = GetModuleHandleA("opengl32.dll");
        if (gl) p = (void*)GetProcAddress(gl, name);
    }
    return p;
}

#define LOAD(name)  \
    name = (PFN##name##PROC)loadProc(#name); \
    if (!name) { GfError("gl3LoadFunctions: missing '%s'\n", #name); ok = false; }

bool gl3LoadFunctions()
{
    bool ok = true;

    LOAD(glCreateShader)
    LOAD(glShaderSource)
    LOAD(glCompileShader)
    LOAD(glGetShaderiv)
    LOAD(glGetShaderInfoLog)
    LOAD(glDeleteShader)
    LOAD(glCreateProgram)
    LOAD(glAttachShader)
    LOAD(glDetachShader)
    LOAD(glLinkProgram)
    LOAD(glGetProgramiv)
    LOAD(glGetProgramInfoLog)
    LOAD(glDeleteProgram)
    LOAD(glUseProgram)
    LOAD(glGetUniformLocation)
    LOAD(glUniform1i)
    LOAD(glUniform1f)
    LOAD(glUniform2f)
    LOAD(glUniform3f)
    LOAD(glUniform4f)
    LOAD(glUniformMatrix4fv)
    LOAD(glGenVertexArrays)
    LOAD(glBindVertexArray)
    LOAD(glDeleteVertexArrays)
    LOAD(glGenBuffers)
    LOAD(glBindBuffer)
    LOAD(glBufferData)
    LOAD(glBufferSubData)
    LOAD(glDeleteBuffers)
    LOAD(glVertexAttribPointer)
    LOAD(glEnableVertexAttribArray)
    LOAD(glDisableVertexAttribArray)
    LOAD(glGenFramebuffers)
    LOAD(glBindFramebuffer)
    LOAD(glFramebufferTexture2D)
    LOAD(glDeleteFramebuffers)
    LOAD(glCheckFramebufferStatus)
    LOAD(glGenRenderbuffers)
    LOAD(glBindRenderbuffer)
    LOAD(glRenderbufferStorage)
    LOAD(glFramebufferRenderbuffer)
    LOAD(glDeleteRenderbuffers)
    LOAD(glActiveTexture)
    LOAD(glGenerateMipmap)
    LOAD(glDrawBuffers)
    LOAD(glDrawElementsInstanced)

#undef LOAD

    if (ok)
        GfOut("gl3LoadFunctions: All OpenGL 3.3 functions loaded successfully.\n");
    else
        GfError("gl3LoadFunctions: One or more GL 3.3 functions missing — renderer may crash.\n");

    return ok;
}

#endif // _WIN32
