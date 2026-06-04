#ifndef RMLUI_MAIN_H
#define RMLUI_MAIN_H

namespace RmlUiMain {
    bool Init();
    void Shutdown();
    void UpdateAndRender(int screenWidth, int screenHeight);
    bool ProcessEvent(void* sdlEvent);
    bool IsActive();
    void SetActive(bool active);
    void LoadRmlMenu(const char* filename);
}

#endif // RMLUI_MAIN_H
