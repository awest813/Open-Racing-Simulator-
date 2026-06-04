/***************************************************************************
    sdl2glut.h -- Minimal GLUT API for TORCS (SDL2 backend, Phase 0 PoC)
***************************************************************************/

#ifndef __SDL2GLUT_H__
#define __SDL2GLUT_H__

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef APIENTRY
# define APIENTRY
#endif

/* Display mode bits (GLUT 3) */
#define GLUT_RGB                        0
#define GLUT_RGBA                       GLUT_RGB
#define GLUT_INDEX                      1
#define GLUT_SINGLE                     0
#define GLUT_DOUBLE                     2
#define GLUT_ACCUM                      4
#define GLUT_ALPHA                      8
#define GLUT_DEPTH                      16
#define GLUT_STENCIL                    32
#define GLUT_MULTISAMPLE                128
#define GLUT_STEREO                     256
#define GLUT_LUMINANCE                  512

/* Mouse */
#define GLUT_LEFT_BUTTON                0
#define GLUT_MIDDLE_BUTTON              1
#define GLUT_RIGHT_BUTTON               2
#define GLUT_DOWN                       0
#define GLUT_UP                         1

/* Special keys */
#define GLUT_KEY_F1                     1
#define GLUT_KEY_F2                     2
#define GLUT_KEY_F3                     3
#define GLUT_KEY_F4                     4
#define GLUT_KEY_F5                     5
#define GLUT_KEY_F6                     6
#define GLUT_KEY_F7                     7
#define GLUT_KEY_F8                     8
#define GLUT_KEY_F9                     9
#define GLUT_KEY_F10                    10
#define GLUT_KEY_F11                    11
#define GLUT_KEY_F12                    12
#define GLUT_KEY_LEFT                   100
#define GLUT_KEY_UP                     101
#define GLUT_KEY_RIGHT                  102
#define GLUT_KEY_DOWN                   103
#define GLUT_KEY_PAGE_UP                104
#define GLUT_KEY_PAGE_DOWN              105
#define GLUT_KEY_HOME                   106
#define GLUT_KEY_END                    107
#define GLUT_KEY_INSERT                 108

/* glutGet / glutGameModeGet */
#define GLUT_WINDOW_WIDTH               0x0066
#define GLUT_WINDOW_HEIGHT              0x0067
#define GLUT_WINDOW_ALPHA_SIZE          0x006E
#define GLUT_DISPLAY_MODE_POSSIBLE      0x0190

#define GLUT_GAME_MODE_POSSIBLE         0x0001
#define GLUT_GAME_MODE_DISPLAY_CHANGED  0x0006

/* Modifiers */
#define GLUT_ACTIVE_SHIFT               0x0001
#define GLUT_ACTIVE_CTRL                0x0002
#define GLUT_ACTIVE_ALT                 0x0004

/* Cursor */
#define GLUT_CURSOR_NONE                0x0065

/* Key repeat (unused stubs) */
#define GLUT_KEY_REPEAT_OFF             0x0000
#define GLUT_KEY_REPEAT_ON              0x0001

#ifdef __cplusplus
extern "C" {
#endif

void APIENTRY glutInit(int *argcp, char **argv);
void APIENTRY glutInitDisplayMode(unsigned int mode);
void APIENTRY glutInitDisplayString(const char *string);
void APIENTRY glutInitWindowPosition(int x, int y);
void APIENTRY glutInitWindowSize(int width, int height);
void APIENTRY glutMainLoop(void);

int APIENTRY glutCreateWindow(const char *title);
void APIENTRY glutPostRedisplay(void);
void APIENTRY glutSwapBuffers(void);
void APIENTRY glutFullScreen(void);

void APIENTRY glutDisplayFunc(void (*func)(void));
void APIENTRY glutReshapeFunc(void (*func)(int width, int height));
void APIENTRY glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
void APIENTRY glutSpecialFunc(void (*func)(int key, int x, int y));
void APIENTRY glutKeyboardUpFunc(void (*func)(unsigned char key, int x, int y));
void APIENTRY glutSpecialUpFunc(void (*func)(int key, int x, int y));
void APIENTRY glutMouseFunc(void (*func)(int button, int state, int x, int y));
void APIENTRY glutMotionFunc(void (*func)(int x, int y));
void APIENTRY glutPassiveMotionFunc(void (*func)(int x, int y));
void APIENTRY glutIdleFunc(void (*func)(void));
void APIENTRY glutTimerFunc(unsigned int millis, void (*func)(int value), int value);

void APIENTRY glutSetCursor(int cursor);
void APIENTRY glutWarpPointer(int x, int y);

int APIENTRY glutGet(GLenum type);
int APIENTRY glutGetModifiers(void);
int APIENTRY glutExtensionSupported(const char *name);

void APIENTRY glutGameModeString(const char *string);
int APIENTRY glutEnterGameMode(void);
void APIENTRY glutLeaveGameMode(void);
int APIENTRY glutGameModeGet(GLenum mode);
void GfuiRegisterImGuiEventHandler(bool (*handler)(void*));

#ifdef __cplusplus
}
#endif

#endif /* __SDL2GLUT_H__ */
