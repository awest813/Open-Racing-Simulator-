/***************************************************************************
 * ImGuiOverlay.h  --  Real-time telemetry & tuning overlay
 *
 * Wraps the Dear ImGui SDL2 + OpenGL 3 backend lifecycle so that the rest
 * of the renderer only needs to call three functions:
 *   ImGuiOverlay::init()      -- once, after GL context creation
 *   ImGuiOverlay::render()    -- every frame, after the scene is drawn
 *   ImGuiOverlay::shutdown()  -- once, on exit
 *
 * The overlay is toggled on/off with F12 and shows:
 *   • RPM bar & gear indicator
 *   • Wheel slip / tire-temperature visualiser
 *   • G-force meter
 *   • Renderer tuning knobs (HDR exposure, bloom)
 ***************************************************************************/
#pragma once

// Pull in the canonical tSituation definition from the TORCS interfaces.
// This avoids the "redefinition / unrelated types" error that arises when a
// bare forward declaration `struct tSituation;` conflicts with the typedef
// defined in raceman.h.
#include <raceman.h>

class OGLRenderer;   // forward, to reach tuning fields

namespace ImGuiOverlay {

/** Initialise Dear ImGui.
 *  @param sdlWindow   The SDL_Window* cast to void* (avoids pulling SDL2
 *                     into every TU that includes this header).
 *  @param glContext   The SDL_GLContext cast to void*.
 *  @param renderer    Pointer to the live OGLRenderer (may be nullptr if
 *                     the overlay is used outside a race).
 */
void init(void* sdlWindow, void* glContext, OGLRenderer* renderer);

/** Render one ImGui frame on top of the already-composited scene. */
void render(const tSituation* sit);

/** Process an SDL_Event (call before your own event dispatch).
 *  Returns true when ImGui consumed the event.
 */
bool processEvent(void* sdlEvent);

/** Free all ImGui resources. */
void shutdown();

/** True when the overlay is currently visible. */
bool isVisible();

} // namespace ImGuiOverlay
