/***************************************************************************
    sdl2glut.cpp -- SDL2-backed minimal GLUT API (Phase 0 PoC)
***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef TORCS_USE_SDL2

#include "sdl2glut.h"

#include <SDL2/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

struct TimerEntry {
	unsigned int fireMs = 0;
	unsigned int intervalMs = 0;
	void (*callback)(int) = nullptr;
	int value = 0;
	bool active = false;
};

SDL_Window *gWindow = nullptr;
SDL_GLContext gGLContext = nullptr;

bool gRunning = true;
bool gNeedsRedisplay = false;
bool gInGameMode = false;
bool gDisplayModePossible = true;
unsigned int gDisplayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;

int gInitX = 0;
int gInitY = 0;
int gInitW = 640;
int gInitH = 480;
int gWindowId = 1;

void (*gDisplayFunc)(void) = nullptr;
void (*gReshapeFunc)(int, int) = nullptr;
void (*gKeyboardFunc)(unsigned char, int, int) = nullptr;
void (*gSpecialFunc)(int, int, int) = nullptr;
void (*gKeyboardUpFunc)(unsigned char, int, int) = nullptr;
void (*gSpecialUpFunc)(int, int, int) = nullptr;
void (*gMouseFunc)(int, int, int, int) = nullptr;
void (*gMotionFunc)(int, int) = nullptr;
void (*gPassiveMotionFunc)(int, int) = nullptr;
void (*gIdleFunc)(void) = nullptr;

std::vector<TimerEntry> gTimers;

unsigned int nowMs()
{
	return static_cast<unsigned int>(SDL_GetTicks());
}

void scheduleTimer(unsigned int millis, void (*func)(int), int value)
{
	TimerEntry entry;
	entry.fireMs = nowMs() + millis;
	entry.intervalMs = millis;
	entry.callback = func;
	entry.value = value;
	entry.active = true;
	gTimers.push_back(entry);
}

void processTimers()
{
	const unsigned int t = nowMs();
	for (TimerEntry &entry : gTimers) {
		if (!entry.active || entry.callback == nullptr) {
			continue;
		}
		if (t >= entry.fireMs) {
			entry.callback(entry.value);
			if (entry.intervalMs > 0) {
				entry.fireMs = t + entry.intervalMs;
			} else {
				entry.active = false;
			}
		}
	}
}

int mapSdlKeyToGlutSpecial(SDL_Keycode key)
{
	switch (key) {
	case SDLK_F1: return GLUT_KEY_F1;
	case SDLK_F2: return GLUT_KEY_F2;
	case SDLK_F3: return GLUT_KEY_F3;
	case SDLK_F4: return GLUT_KEY_F4;
	case SDLK_F5: return GLUT_KEY_F5;
	case SDLK_F6: return GLUT_KEY_F6;
	case SDLK_F7: return GLUT_KEY_F7;
	case SDLK_F8: return GLUT_KEY_F8;
	case SDLK_F9: return GLUT_KEY_F9;
	case SDLK_F10: return GLUT_KEY_F10;
	case SDLK_F11: return GLUT_KEY_F11;
	case SDLK_F12: return GLUT_KEY_F12;
	case SDLK_LEFT: return GLUT_KEY_LEFT;
	case SDLK_UP: return GLUT_KEY_UP;
	case SDLK_RIGHT: return GLUT_KEY_RIGHT;
	case SDLK_DOWN: return GLUT_KEY_DOWN;
	case SDLK_PAGEUP: return GLUT_KEY_PAGE_UP;
	case SDLK_PAGEDOWN: return GLUT_KEY_PAGE_DOWN;
	case SDLK_HOME: return GLUT_KEY_HOME;
	case SDLK_END: return GLUT_KEY_END;
	case SDLK_INSERT: return GLUT_KEY_INSERT;
	default: return -1;
	}
}

int mapSdlButton(int button)
{
	switch (button) {
	case SDL_BUTTON_LEFT: return GLUT_LEFT_BUTTON;
	case SDL_BUTTON_MIDDLE: return GLUT_MIDDLE_BUTTON;
	case SDL_BUTTON_RIGHT: return GLUT_RIGHT_BUTTON;
	default: return GLUT_LEFT_BUTTON;
	}
}

void dispatchKeyboard(SDL_Event &ev, bool keyUp)
{
	const int special = mapSdlKeyToGlutSpecial(ev.key.keysym.sym);
	int x = 0;
	int y = 0;
	SDL_GetMouseState(&x, &y);

	if (special >= 0) {
		if (keyUp) {
			if (gSpecialUpFunc) {
				gSpecialUpFunc(special, x, y);
			}
		} else if (gSpecialFunc) {
			gSpecialFunc(special, x, y);
		}
		return;
	}

	if ((ev.key.keysym.sym >= 32) && (ev.key.keysym.sym < 127)) {
		const unsigned char key = static_cast<unsigned char>(ev.key.keysym.sym);
		if (keyUp) {
			if (gKeyboardUpFunc) {
				gKeyboardUpFunc(key, x, y);
			}
		} else if (gKeyboardFunc) {
			gKeyboardFunc(key, x, y);
		}
	}
}

void dispatchMouse(SDL_Event &ev)
{
	if (!gMouseFunc) {
		return;
	}

	const int button = mapSdlButton(ev.button.button);
	const int state = (ev.type == SDL_MOUSEBUTTONDOWN) ? GLUT_DOWN : GLUT_UP;
	gMouseFunc(button, state, ev.button.x, ev.button.y);
}

void dispatchMotion(SDL_Event &ev, bool passive)
{
	if (passive) {
		if (gPassiveMotionFunc) {
			gPassiveMotionFunc(ev.motion.x, ev.motion.y);
		}
	} else if (gMotionFunc) {
		gMotionFunc(ev.motion.x, ev.motion.y);
	}
}

void applyGlAttributes()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, (gDisplayMode & GLUT_DOUBLE) ? 1 : 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, (gDisplayMode & GLUT_DEPTH) ? 24 : 0);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, (gDisplayMode & GLUT_ALPHA) ? 8 : 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, (gDisplayMode & GLUT_STENCIL) ? 8 : 0);
	if (gDisplayMode & GLUT_MULTISAMPLE) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
}

bool createGlContext()
{
	if (gWindow == nullptr) {
		return false;
	}

	gGLContext = SDL_GL_CreateContext(gWindow);
	if (gGLContext == nullptr) {
		std::fprintf(stderr, "SDL2 GLUT: SDL_GL_CreateContext failed: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_MakeCurrent(gWindow, gGLContext);
	return true;
}

} // namespace

extern "C" {

void APIENTRY glutInit(int *argcp, char **argv)
{
	(void)argcp;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		std::fprintf(stderr, "SDL2 GLUT: SDL_Init failed: %s\n", SDL_GetError());
		std::exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
}

void APIENTRY glutInitDisplayMode(unsigned int mode)
{
	gDisplayMode = mode;
	gDisplayModePossible = true;
}

void APIENTRY glutInitDisplayString(const char *string)
{
	(void)string;
	/* Phase 0: accept requested mode; real visual selection is via SDL_GL_SetAttribute at context creation. */
	gDisplayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA;
	if (string != nullptr) {
		if (std::strstr(string, "samples") != nullptr) {
			gDisplayMode |= GLUT_MULTISAMPLE;
		}
	}
	gDisplayModePossible = true;
}

void APIENTRY glutInitWindowPosition(int x, int y)
{
	gInitX = x;
	gInitY = y;
}

void APIENTRY glutInitWindowSize(int width, int height)
{
	gInitW = width;
	gInitH = height;
}

int APIENTRY glutCreateWindow(const char *title)
{
	applyGlAttributes();

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	gWindow = SDL_CreateWindow(
		(title != nullptr) ? title : "TORCS",
		gInitX,
		gInitY,
		gInitW,
		gInitH,
		flags);

	if (gWindow == nullptr) {
		std::fprintf(stderr, "SDL2 GLUT: SDL_CreateWindow failed: %s\n", SDL_GetError());
		return 0;
	}

	if (!createGlContext()) {
		SDL_DestroyWindow(gWindow);
		gWindow = nullptr;
		return 0;
	}

	return gWindowId;
}

void APIENTRY glutPostRedisplay(void)
{
	gNeedsRedisplay = true;
}

void APIENTRY glutSwapBuffers(void)
{
	if (gWindow != nullptr) {
		SDL_GL_SwapWindow(gWindow);
	}
}

void APIENTRY glutFullScreen(void)
{
	if (gWindow != nullptr) {
		SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
}

void APIENTRY glutDisplayFunc(void (*func)(void))
{
	gDisplayFunc = func;
}

void APIENTRY glutReshapeFunc(void (*func)(int width, int height))
{
	gReshapeFunc = func;
}

void APIENTRY glutKeyboardFunc(void (*func)(unsigned char key, int x, int y))
{
	gKeyboardFunc = func;
}

void APIENTRY glutSpecialFunc(void (*func)(int key, int x, int y))
{
	gSpecialFunc = func;
}

void APIENTRY glutKeyboardUpFunc(void (*func)(unsigned char key, int x, int y))
{
	gKeyboardUpFunc = func;
}

void APIENTRY glutSpecialUpFunc(void (*func)(int key, int x, int y))
{
	gSpecialUpFunc = func;
}

void APIENTRY glutMouseFunc(void (*func)(int button, int state, int x, int y))
{
	gMouseFunc = func;
}

void APIENTRY glutMotionFunc(void (*func)(int x, int y))
{
	gMotionFunc = func;
}

void APIENTRY glutPassiveMotionFunc(void (*func)(int x, int y))
{
	gPassiveMotionFunc = func;
}

void APIENTRY glutIdleFunc(void (*func)(void))
{
	gIdleFunc = func;
}

void APIENTRY glutTimerFunc(unsigned int millis, void (*func)(int value), int value)
{
	scheduleTimer(millis, func, value);
}

void APIENTRY glutSetCursor(int cursor)
{
	if (gWindow == nullptr) {
		return;
	}
	if (cursor == GLUT_CURSOR_NONE) {
		SDL_ShowCursor(SDL_DISABLE);
	} else {
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void APIENTRY glutWarpPointer(int x, int y)
{
	if (gWindow != nullptr) {
		SDL_WarpMouseInWindow(gWindow, x, y);
	}
}

int APIENTRY glutGet(GLenum type)
{
	int w = 0;
	int h = 0;

	if (gWindow != nullptr) {
		SDL_GetWindowSize(gWindow, &w, &h);
	}

	switch (type) {
	case GLUT_WINDOW_WIDTH:
		return w;
	case GLUT_WINDOW_HEIGHT:
		return h;
	case GLUT_WINDOW_ALPHA_SIZE:
		return (gDisplayMode & GLUT_ALPHA) ? 8 : 0;
	case GLUT_DISPLAY_MODE_POSSIBLE:
		return gDisplayModePossible ? 1 : 0;
	default:
		return 0;
	}
}

int APIENTRY glutGetModifiers(void)
{
	const Uint16 mod = SDL_GetModState();
	int result = 0;
	if (mod & KMOD_SHIFT) {
		result |= GLUT_ACTIVE_SHIFT;
	}
	if (mod & KMOD_CTRL) {
		result |= GLUT_ACTIVE_CTRL;
	}
	if (mod & KMOD_ALT) {
		result |= GLUT_ACTIVE_ALT;
	}
	return result;
}

int APIENTRY glutExtensionSupported(const char *name)
{
	if ((name == nullptr) || (gGLContext == nullptr)) {
		return 0;
	}

	const char *extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
	if (extensions == nullptr) {
		return 0;
	}

	return (std::strstr(extensions, name) != nullptr) ? 1 : 0;
}

void APIENTRY glutGameModeString(const char *string)
{
	(void)string;
}

int APIENTRY glutEnterGameMode(void)
{
	if (gWindow == nullptr) {
		applyGlAttributes();
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP |
			SDL_WINDOW_ALLOW_HIGHDPI;
		gWindow = SDL_CreateWindow("TORCS", gInitX, gInitY, gInitW, gInitH, flags);
		if (gWindow == nullptr) {
			std::fprintf(stderr, "SDL2 GLUT: game mode window failed: %s\n", SDL_GetError());
			return 0;
		}
		if (!createGlContext()) {
			SDL_DestroyWindow(gWindow);
			gWindow = nullptr;
			return 0;
		}
		gInGameMode = true;
		return 1;
	}

	if (SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
		gInGameMode = true;
		return 1;
	}
	return 0;
}

void APIENTRY glutLeaveGameMode(void)
{
	if (gWindow != nullptr) {
		SDL_SetWindowFullscreen(gWindow, 0);
	}
	gInGameMode = false;
}

int APIENTRY glutGameModeGet(GLenum mode)
{
	switch (mode) {
	case GLUT_GAME_MODE_POSSIBLE:
		return 1;
	case GLUT_GAME_MODE_DISPLAY_CHANGED:
		return gInGameMode ? 1 : 0;
	default:
		return 0;
	}
}

void APIENTRY glutMainLoop(void)
{
	while (gRunning) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				gRunning = false;
				break;
			case SDL_WINDOWEVENT:
				if ((ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) && gReshapeFunc) {
					gReshapeFunc(ev.window.data1, ev.window.data2);
				}
				break;
			case SDL_KEYDOWN:
				dispatchKeyboard(ev, false);
				gNeedsRedisplay = true;
				break;
			case SDL_KEYUP:
				dispatchKeyboard(ev, true);
				gNeedsRedisplay = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				dispatchMouse(ev);
				gNeedsRedisplay = true;
				break;
			case SDL_MOUSEMOTION:
				dispatchMotion(ev, (ev.motion.state & SDL_BUTTON_LMASK) == 0 &&
					(ev.motion.state & SDL_BUTTON_MMASK) == 0 &&
					(ev.motion.state & SDL_BUTTON_RMASK) == 0);
				break;
			default:
				break;
			}
		}

		processTimers();

		if (gIdleFunc) {
			gIdleFunc();
		}

		if (gNeedsRedisplay && gDisplayFunc) {
			gNeedsRedisplay = false;
			gDisplayFunc();
		}

		SDL_Delay(1);
	}

	if (gGLContext != nullptr) {
		SDL_GL_DeleteContext(gGLContext);
		gGLContext = nullptr;
	}
	if (gWindow != nullptr) {
		SDL_DestroyWindow(gWindow);
		gWindow = nullptr;
	}
	SDL_Quit();
}

} // extern "C"

#endif /* TORCS_USE_SDL2 */
