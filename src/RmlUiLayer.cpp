#include "RmlUiLayer.h"

#include "raylib.h"
#include "rlgl.h"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

// ---------------------------------------------------------------------------
// Raylib key → RmlUi key mapping
// ---------------------------------------------------------------------------
static Rml::Input::KeyIdentifier MapKey(int key) {
    if (key >= KEY_A && key <= KEY_Z)
        return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_A + (key - KEY_A));
    if (key >= KEY_ZERO && key <= KEY_NINE)
        return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_0 + (key - KEY_ZERO));
    if (key >= KEY_F1 && key <= KEY_F12)
        return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_F1 + (key - KEY_F1));
    if (key >= KEY_KP_0 && key <= KEY_KP_9)
        return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_NUMPAD0 + (key - KEY_KP_0));

    switch (key) {
        case KEY_ENTER:        return Rml::Input::KI_RETURN;
        case KEY_ESCAPE:       return Rml::Input::KI_ESCAPE;
        case KEY_TAB:          return Rml::Input::KI_TAB;
        case KEY_BACKSPACE:    return Rml::Input::KI_BACK;
        case KEY_DELETE:       return Rml::Input::KI_DELETE;
        case KEY_INSERT:       return Rml::Input::KI_INSERT;
        case KEY_LEFT:         return Rml::Input::KI_LEFT;
        case KEY_RIGHT:        return Rml::Input::KI_RIGHT;
        case KEY_UP:           return Rml::Input::KI_UP;
        case KEY_DOWN:         return Rml::Input::KI_DOWN;
        case KEY_HOME:         return Rml::Input::KI_HOME;
        case KEY_END:          return Rml::Input::KI_END;
        case KEY_PAGE_UP:      return Rml::Input::KI_PRIOR;
        case KEY_PAGE_DOWN:    return Rml::Input::KI_NEXT;
        case KEY_SPACE:        return Rml::Input::KI_SPACE;
        case KEY_LEFT_SHIFT:   return Rml::Input::KI_LSHIFT;
        case KEY_RIGHT_SHIFT:  return Rml::Input::KI_RSHIFT;
        case KEY_LEFT_CONTROL: return Rml::Input::KI_LCONTROL;
        case KEY_RIGHT_CONTROL:return Rml::Input::KI_RCONTROL;
        case KEY_LEFT_ALT:     return Rml::Input::KI_LMENU;
        case KEY_RIGHT_ALT:    return Rml::Input::KI_RMENU;
        default:               return Rml::Input::KI_UNKNOWN;
    }
}

static int GetModifiers() {
    int m = 0;
    if (IsKeyDown(KEY_LEFT_SHIFT)   || IsKeyDown(KEY_RIGHT_SHIFT))   m |= Rml::Input::KM_SHIFT;
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) m |= Rml::Input::KM_CTRL;
    if (IsKeyDown(KEY_LEFT_ALT)     || IsKeyDown(KEY_RIGHT_ALT))     m |= Rml::Input::KM_ALT;
    return m;
}

// ---------------------------------------------------------------------------
// RmlUiLayer implementation
// ---------------------------------------------------------------------------
void RmlUiLayer::Init(int width, int height) {
    RmlGL3::Initialize();

    Rml::SetSystemInterface(&sys_iface);
    Rml::SetRenderInterface(&render_iface);
    Rml::Initialise();

    sys_iface.LoadCursors();

    render_iface.SetViewport(width, height);
    ctx = Rml::CreateContext("main", Rml::Vector2i(width, height));
    Rml::Debugger::Initialise(ctx);
}

void RmlUiLayer::Shutdown() {
    sys_iface.DestroyCursors();
    Rml::Shutdown();
    RmlGL3::Shutdown();
}

void RmlUiLayer::OnResize(int width, int height) {
    render_iface.SetViewport(width, height);
    ctx->SetDimensions(Rml::Vector2i(width, height));
}

void RmlUiLayer::PumpInput() {
    int mods = GetModifiers();

    ctx->ProcessMouseMove(GetMouseX(), GetMouseY(), mods);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))    ctx->ProcessMouseButtonDown(0, mods);
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))   ctx->ProcessMouseButtonUp(0, mods);
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))   ctx->ProcessMouseButtonDown(1, mods);
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))  ctx->ProcessMouseButtonUp(1, mods);
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))  ctx->ProcessMouseButtonDown(2, mods);
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) ctx->ProcessMouseButtonUp(2, mods);

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) ctx->ProcessMouseWheel(wheel, mods);

    for (int key = GetKeyPressed(); key != 0; key = GetKeyPressed()) {
        auto rk = MapKey(key);
        if (rk != Rml::Input::KI_UNKNOWN)
            ctx->ProcessKeyDown(rk, mods);
    }

    for (int ch = GetCharPressed(); ch != 0; ch = GetCharPressed())
        ctx->ProcessTextInput(Rml::Character(ch));
}

void RmlUiLayer::Update() {
    ctx->Update();
}

void RmlUiLayer::Render() {
    rlDrawRenderBatchActive();
    render_iface.BeginFrame();
    ctx->Render();
    render_iface.EndFrame();
}

void RmlUiLayer::ToggleDebugger() {
    Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
}
