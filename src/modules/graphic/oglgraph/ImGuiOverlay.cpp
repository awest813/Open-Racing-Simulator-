/***************************************************************************
 * ImGuiOverlay.cpp  --  Forza-style Dear ImGui telemetry/tuning overlay
 *
 * Design language:
 *   Dark glass panels  (#0A0A0F / 85% opacity)
 *   Neon accent        (#FF6A00  orange / #00C8FF cyan)
 *   Typography         ImGui default (Proggy), 14 px base
 *
 * The overlay renders ONLY when TORCS_USE_SDL2 is defined because we rely
 * on the SDL2 + OpenGL3 ImGui back-ends.  In legacy GLUT builds the stub
 * implementations below compile to nothing.
 ***************************************************************************/

#include "ImGuiOverlay.h"
#include "OGLRenderer.h"

#ifdef TORCS_USE_SDL2

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <cmath>
#include <cstring>
#include <algorithm>

// TORCS race situation struct (includes car, driver, timing data)
#include <raceman.h>

namespace {

// ---------- module state --------------------------------------------------
static SDL_Window*  g_sdlWindow = nullptr;
static OGLRenderer* g_renderer  = nullptr;
static bool         g_visible   = false;   // toggled by F12
static bool         g_inited    = false;

// ---------- colour helpers ------------------------------------------------
static inline ImVec4 hexRGBA(unsigned r, unsigned g, unsigned b, unsigned a = 255)
{
    return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

static const ImVec4 kPanelBg  = hexRGBA( 10,  10,  15, 215);
static const ImVec4 kAccentOr = hexRGBA(255, 106,   0, 255);   // orange
static const ImVec4 kAccentCy = hexRGBA(  0, 200, 255, 255);   // cyan
static const ImVec4 kAccentGr = hexRGBA( 50, 220,  50, 255);   // green
static const ImVec4 kAccentRd = hexRGBA(255,  50,  50, 255);   // red
static const ImVec4 kTextDim  = hexRGBA(160, 160, 180, 255);
static const ImVec4 kTextBrt  = hexRGBA(240, 240, 255, 255);

// ---------- styling -------------------------------------------------------
static void applyForzaTheme()
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 8.0f;
    s.FrameRounding     = 4.0f;
    s.GrabRounding      = 4.0f;
    s.ScrollbarRounding = 4.0f;
    s.WindowBorderSize  = 0.0f;
    s.FrameBorderSize   = 1.0f;
    s.ItemSpacing       = ImVec2(8.f, 6.f);
    s.WindowPadding     = ImVec2(12.f, 10.f);

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]         = kPanelBg;
    c[ImGuiCol_TitleBg]          = hexRGBA( 20, 20,  30, 230);
    c[ImGuiCol_TitleBgActive]    = hexRGBA( 30, 30,  50, 255);
    c[ImGuiCol_FrameBg]          = hexRGBA( 20, 20,  30, 200);
    c[ImGuiCol_FrameBgHovered]   = hexRGBA( 40, 40,  60, 200);
    c[ImGuiCol_FrameBgActive]    = hexRGBA( 60, 60,  90, 200);
    c[ImGuiCol_SliderGrab]       = kAccentOr;
    c[ImGuiCol_SliderGrabActive] = hexRGBA(255, 140, 40, 255);
    c[ImGuiCol_Button]           = hexRGBA( 50,  50,  80, 200);
    c[ImGuiCol_ButtonHovered]    = hexRGBA( 80,  80, 120, 220);
    c[ImGuiCol_ButtonActive]     = kAccentOr;
    c[ImGuiCol_Header]           = hexRGBA( 40,  40,  70, 200);
    c[ImGuiCol_HeaderHovered]    = hexRGBA( 60,  60, 100, 220);
    c[ImGuiCol_HeaderActive]     = kAccentOr;
    c[ImGuiCol_Text]             = kTextBrt;
    c[ImGuiCol_TextDisabled]     = kTextDim;
    c[ImGuiCol_Border]           = hexRGBA( 80,  80, 120, 160);
    c[ImGuiCol_Separator]        = hexRGBA( 80,  80, 120, 160);
    c[ImGuiCol_CheckMark]        = kAccentOr;
    c[ImGuiCol_PlotLines]        = kAccentCy;
    c[ImGuiCol_PlotHistogram]    = kAccentOr;
    c[ImGuiCol_ScrollbarBg]      = hexRGBA( 10,  10,  20, 180);
    c[ImGuiCol_ScrollbarGrab]    = hexRGBA( 60,  60, 100, 200);
}

// ---------- helpers -------------------------------------------------------

/// Gradient horizontal bar: value in [0..1], coloured by barCol.
static void coloredBar(const char* label, float value, ImVec4 barCol,
                       float height = 16.f)
{
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      pos   = ImGui::GetCursorScreenPos();
    float       avail = ImGui::GetContentRegionAvail().x;
    float       fill  = avail * std::max(0.f, std::min(1.f, value));

    // Background track
    dl->AddRectFilled(pos, ImVec2(pos.x + avail, pos.y + height),
                      IM_COL32(30, 30, 50, 200), 4.f);
    // Filled portion — left-darker to right-brighter gradient for glow feel
    if (fill > 0.f) {
        const ImU32 colDim = IM_COL32((int)(barCol.x * 180),
                                      (int)(barCol.y * 180),
                                      (int)(barCol.z * 180), 255);
        const ImU32 colBrt = IM_COL32((int)(barCol.x * 255),
                                      (int)(barCol.y * 255),
                                      (int)(barCol.z * 255), 255);
        dl->AddRectFilledMultiColor(
            pos, ImVec2(pos.x + fill, pos.y + height),
            colDim, colBrt, colBrt, colDim);
    }
    // Label centred vertically inside the bar
    if (label && label[0]) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%s  %.0f%%", label, value * 100.f);
        ImVec2 tp = ImVec2(pos.x + 4.f,
                           pos.y + (height - ImGui::GetTextLineHeight()) * 0.5f);
        dl->AddText(tp, IM_COL32(220, 220, 240, 255), buf);
    }
    ImGui::Dummy(ImVec2(avail, height + 4.f));
}

/// Circular G-force indicator.
static void gForceMeter(float gLat, float gLon, float radius = 50.f)
{
    ImDrawList* dl   = ImGui::GetWindowDrawList();
    ImVec2      cpos = ImGui::GetCursorScreenPos();
    float       cx   = cpos.x + radius + 4.f;
    float       cy   = cpos.y + radius + 4.f;
    float       scale = radius / 3.0f;  // 3G = edge of circle

    // Outer ring + 1G inner ring guide
    dl->AddCircle(ImVec2(cx, cy), radius,
                  IM_COL32(80, 80, 120, 180), 64, 1.5f);
    dl->AddCircle(ImVec2(cx, cy), radius / 3.f,
                  IM_COL32(60, 60, 90, 100), 32, 1.f);
    // Cross-hairs
    dl->AddLine(ImVec2(cx - radius, cy), ImVec2(cx + radius, cy),
                IM_COL32(60, 60, 80, 100), 1.f);
    dl->AddLine(ImVec2(cx, cy - radius), ImVec2(cx, cy + radius),
                IM_COL32(60, 60, 80, 100), 1.f);

    // Dot — clamped inside circle
    float dotX = gLat * scale;
    float dotY = -gLon * scale;   // positive = braking (up on screen)
    float dist  = std::sqrt(dotX * dotX + dotY * dotY);
    float maxR  = radius - 6.f;
    if (dist > maxR && dist > 0.f) {
        dotX = dotX / dist * maxR;
        dotY = dotY / dist * maxR;
    }
    // Colour: transitions red (lateral) → cyan (longitudinal)
    float tLat = std::min(1.f, std::abs(gLat) / 3.f);
    ImU32 dotColor = IM_COL32(
        (int)(255 * tLat),
        (int)(50  * (1.f - tLat)),
        (int)(255 * (1.f - tLat)),
        255);
    dl->AddCircleFilled(ImVec2(cx + dotX, cy + dotY), 7.f, dotColor);
    // Soft glow halo
    dl->AddCircle(ImVec2(cx + dotX, cy + dotY), 11.f,
                  (dotColor & 0x00FFFFFF) | 0x50000000, 24, 2.5f);

    ImGui::Dummy(ImVec2((radius + 4.f) * 2.f, (radius + 4.f) * 2.f));
}

// ---------- telemetry window ----------------------------------------------
static void drawTelemetry(const tSituation* sit)
{
    // Grab the player car — prefer the human driver, fall back to index 0
    const tCarElt* car = nullptr;
    if (sit && sit->cars && sit->raceInfo.ncars > 0) {
        car = sit->cars[0];
        for (int i = 0; i < sit->raceInfo.ncars; ++i) {
            if (sit->cars[i]->_driverType == RM_DRV_HUMAN) {
                car = sit->cars[i];
                break;
            }
        }
    }

    // ---- Extract data (safe defaults when no car) ----
    float rpm    = car ? (float)car->priv.enginerpm       : 0.f;
    float maxRpm = car ? (float)car->priv.enginerpmMaxTq  : 10000.f;
    if (maxRpm < 1.f) maxRpm = 10000.f;
    float rpmFrac = std::max(0.f, std::min(1.f, rpm / maxRpm));

    int   gear  = car ? car->priv.gear    : 0;
    // pub.speed is the pre-computed total speed magnitude (m/s)
    float speed = car ? (float)car->pub.speed * 3.6f : 0.f;  // km/h

    // Wheel side-slip (normalised 0..1, capped at 5 m/s slip)
    // Wheel order: FRNT_RGT=0, FRNT_LFT=1, REAR_RGT=2, REAR_LFT=3
    float slip[4] = {0.f, 0.f, 0.f, 0.f};
    if (car) {
        for (int i = 0; i < 4; ++i) {
            slip[i] = std::min(1.f,
                               std::abs((float)car->priv.wheel[i].slipSide) / 5.f);
        }
    }

    // G-forces: use car-body axis (DynGC) so they relate to the driver's frame.
    //   acc.x = fore-aft (positive = acceleration, negative = braking)
    //   acc.y = lateral (positive = right, negative = left)
    float gLon = 0.f, gLat = 0.f;
    if (car) {
        gLon = (float)car->pub.DynGC.acc.x / 9.81f;
        gLat = (float)car->pub.DynGC.acc.y / 9.81f;
    }

    // Race info
    int   pos  = car ? car->race.pos         : 0;
    int   lap  = car ? car->race.laps        : 0;
    float lapT = car ? (float)car->race.bestLapTime : 0.f;

    // ---- Window ----
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove      | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2(16.f, 16.f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGui::Begin("TELEMETRY", nullptr, flags);

    // ---- Speed + gear header ----
    ImGui::PushStyleColor(ImGuiCol_Text, kAccentCy);
    ImGui::Text("%.0f", speed);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextColored(kTextDim, "km/h");
    ImGui::SameLine(0.f, 20.f);

    // Gear string
    const char* gearStr;
    if      (gear == -1) gearStr = "R";
    else if (gear ==  0) gearStr = "N";
    else if (gear ==  1) gearStr = "1";
    else if (gear ==  2) gearStr = "2";
    else if (gear ==  3) gearStr = "3";
    else if (gear ==  4) gearStr = "4";
    else if (gear ==  5) gearStr = "5";
    else if (gear ==  6) gearStr = "6";
    else                 gearStr = "7+";

    ImGui::PushStyleColor(ImGuiCol_Text, kAccentOr);
    ImGui::Text("Gear %s", gearStr);
    ImGui::PopStyleColor();

    ImGui::Separator();

    // ---- RPM bar ----
    ImVec4 rpmColor = (rpmFrac > 0.85f) ? kAccentRd :
                      (rpmFrac > 0.65f) ? kAccentOr : kAccentCy;
    coloredBar("RPM", rpmFrac, rpmColor, 20.f);
    ImGui::TextColored(kTextDim, "%.0f / %.0f rpm", rpm, maxRpm);

    ImGui::Separator();

    // ---- Wheel slip ----
    // Labels match TORCS wheel order: FRNT_RGT=0, FRNT_LFT=1, REAR_RGT=2, REAR_LFT=3
    ImGui::TextColored(kTextDim, "WHEEL SLIP");
    static const char* wLabels[] = { "FR", "FL", "RR", "RL" };
    for (int i = 0; i < 4; ++i) {
        ImVec4 sc = (slip[i] > 0.6f) ? kAccentRd :
                    (slip[i] > 0.3f) ? kAccentOr : kAccentGr;
        ImGui::Text("%s", wLabels[i]);
        ImGui::SameLine(30.f);
        coloredBar("", slip[i], sc, 14.f);
    }

    ImGui::Separator();

    // ---- G-force ----
    ImGui::TextColored(kTextDim, "G-FORCE");
    ImGui::SameLine();
    ImGui::Text("Lat %.2fG  Lon %.2fG", gLat, gLon);
    gForceMeter(gLat, gLon, 50.f);

    ImGui::Separator();

    // ---- Race info ----
    ImGui::TextColored(kTextDim, "POS");
    ImGui::SameLine();
    ImGui::Text("%d", pos);
    ImGui::SameLine(0.f, 14.f);
    ImGui::TextColored(kTextDim, "LAP");
    ImGui::SameLine();
    ImGui::Text("%d", lap);

    if (lapT > 0.f) {
        int   mins = (int)(lapT / 60.f);
        float secs = lapT - (float)mins * 60.f;
        ImGui::TextColored(kTextDim, "Best");
        ImGui::SameLine();
        ImGui::Text("%d:%06.3f", mins, secs);
    }

    ImGui::End();
}

// ---------- renderer tuning window ----------------------------------------
static void drawRendererTuning()
{
    if (!g_renderer || !g_sdlWindow) return;

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove      | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoScrollbar;

    int winW = 800, winH = 600;
    SDL_GetWindowSize(g_sdlWindow, &winW, &winH);
    ImGui::SetNextWindowPos(ImVec2((float)winW - 280.f, 16.f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGui::Begin("RENDERER", nullptr, flags);

    ImGui::TextColored(kAccentOr, "HDR / POST-PROCESS");
    ImGui::Separator();
    ImGui::SliderFloat("Exposure",     &g_renderer->m_hdrExposure,  0.1f, 4.0f, "%.2f");
    ImGui::SliderFloat("Bloom Str.",   &g_renderer->m_bloomStrength, 0.0f, 2.0f, "%.2f");
    ImGui::SliderInt  ("Bloom Passes", &g_renderer->m_bloomPasses,   0,    8);

    ImGui::Separator();
    ImGui::TextColored(kAccentOr, "SHADOW MAP");

    // Map current shadow size to combo index
    static const int kSizes[]    = { 512, 1024, 2048, 4096 };
    static const char* kSizeStr  = "512\0" "1024\0" "2048\0" "4096\0";
    static int shadowIdx = 2;
    for (int i = 0; i < 4; ++i) {
        if (kSizes[i] == g_renderer->m_shadowMapSize) { shadowIdx = i; break; }
    }
    if (ImGui::Combo("Shadow Res", &shadowIdx, kSizeStr))
        g_renderer->m_shadowMapSize = kSizes[shadowIdx];

    ImGui::Separator();
    if (ImGui::Button("Reset Defaults")) {
        g_renderer->m_hdrExposure   = 1.0f;
        g_renderer->m_bloomStrength = 0.75f;
        g_renderer->m_bloomPasses   = 5;
        g_renderer->m_shadowMapSize = 2048;
    }

    ImGui::End();
}

} // anonymous namespace

// ===========================================================================
// Public API
// ===========================================================================

namespace ImGuiOverlay {

void init(void* sdlWindow, void* glContext, OGLRenderer* renderer)
{
    if (g_inited) return;   // guard against double-init

    g_sdlWindow = static_cast<SDL_Window*>(sdlWindow);
    g_renderer  = renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename  = nullptr;   // don't persist layout

    applyForzaTheme();

    ImGui_ImplSDL2_InitForOpenGL(g_sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    g_inited = true;
}

void render(const tSituation* sit)
{
    if (!g_inited) return;

    // ---- Start new ImGui frame (needed to poll keys) ----
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // F12 toggle — must happen after NewFrame()
    if (ImGui::IsKeyPressed(ImGuiKey_F12, /*repeat=*/false))
        g_visible = !g_visible;

    if (g_visible) {
        drawTelemetry(sit);
        drawRendererTuning();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool processEvent(void* sdlEvent)
{
    if (!g_inited) return false;
    SDL_Event* ev = static_cast<SDL_Event*>(sdlEvent);
    ImGui_ImplSDL2_ProcessEvent(ev);
    // Only block game input when the overlay is open and ImGui wants it
    if (!g_visible) return false;
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

void shutdown()
{
    if (!g_inited) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    g_sdlWindow = nullptr;
    g_renderer  = nullptr;
    g_inited    = false;
}

bool isVisible() { return g_visible; }

} // namespace ImGuiOverlay

// ===========================================================================
// Stub for legacy GLUT builds
// ===========================================================================
#else  // !TORCS_USE_SDL2

namespace ImGuiOverlay {
void init(void*, void*, OGLRenderer*) {}
void render(const tSituation*)        {}
bool processEvent(void*)              { return false; }
void shutdown()                       {}
bool isVisible()                      { return false; }
} // namespace ImGuiOverlay

#endif // TORCS_USE_SDL2
