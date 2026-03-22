#pragma once

#include "RaylibSystemInterface.h"
#include "RmlUi_Renderer_GL3.h"
#include <RmlUi/Core/Context.h>

// Owns all RmlUi state and exposes a simple per-frame lifecycle.
// Call Init() after InitWindow(), Shutdown() before CloseWindow().
class RmlUiLayer {
public:
    void Init(int width, int height);
    void Shutdown();

    // Call when the window is resized
    void OnResize(int width, int height);

    // Forward raylib input to RmlUi (call before Update)
    void PumpInput();

    // Advance RmlUi layout/animations
    void Update();

    // Flush raylib batch then render RmlUi (call inside BeginDrawing/EndDrawing)
    void Render();

    void ToggleDebugger();

    Rml::Context* GetContext() { return ctx; }

private:
    RaylibSystemInterface sys_iface;
    RenderInterface_GL3   render_iface;
    Rml::Context*         ctx = nullptr;
};
