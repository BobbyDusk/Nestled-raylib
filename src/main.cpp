#include "raylib.h"
#include "rlgl.h"

#include "RmlUi_Renderer_GL3.h"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

// ---------------------------------------------------------------------------
// Minimal RmlUi system interface backed by raylib
// ---------------------------------------------------------------------------
class RaylibSystemInterface : public Rml::SystemInterface {
public:
  double GetElapsedTime() override { return GetTime(); }

  bool LogMessage(Rml::Log::Type type, const Rml::String &message) override {
    int level = LOG_INFO;
    switch (type) {
    case Rml::Log::LT_ERROR:
      level = LOG_ERROR;
      break;
    case Rml::Log::LT_WARNING:
      level = LOG_WARNING;
      break;
    case Rml::Log::LT_INFO:
      level = LOG_INFO;
      break;
    case Rml::Log::LT_DEBUG:
      level = LOG_DEBUG;
      break;
    default:
      break;
    }
    TraceLog(level, "RmlUi: %s", message.c_str());
    return true;
  }

  void SetClipboardText(const Rml::String &text) override {
    ::SetClipboardText(text.c_str());
  }

  void GetClipboardText(Rml::String &text) override {
    text = ::GetClipboardText();
  }
};

// ---------------------------------------------------------------------------
// Raylib key → RmlUi key mapping
// ---------------------------------------------------------------------------
static Rml::Input::KeyIdentifier MapKey(int key) {
  if (key >= KEY_A && key <= KEY_Z)
    return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_A +
                                                  (key - KEY_A));
  if (key >= KEY_ZERO && key <= KEY_NINE)
    return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_0 +
                                                  (key - KEY_ZERO));
  if (key >= KEY_F1 && key <= KEY_F12)
    return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_F1 +
                                                  (key - KEY_F1));
  if (key >= KEY_KP_0 && key <= KEY_KP_9)
    return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_NUMPAD0 +
                                                  (key - KEY_KP_0));

  switch (key) {
  case KEY_ENTER:
    return Rml::Input::KI_RETURN;
  case KEY_ESCAPE:
    return Rml::Input::KI_ESCAPE;
  case KEY_TAB:
    return Rml::Input::KI_TAB;
  case KEY_BACKSPACE:
    return Rml::Input::KI_BACK;
  case KEY_DELETE:
    return Rml::Input::KI_DELETE;
  case KEY_INSERT:
    return Rml::Input::KI_INSERT;
  case KEY_LEFT:
    return Rml::Input::KI_LEFT;
  case KEY_RIGHT:
    return Rml::Input::KI_RIGHT;
  case KEY_UP:
    return Rml::Input::KI_UP;
  case KEY_DOWN:
    return Rml::Input::KI_DOWN;
  case KEY_HOME:
    return Rml::Input::KI_HOME;
  case KEY_END:
    return Rml::Input::KI_END;
  case KEY_PAGE_UP:
    return Rml::Input::KI_PRIOR;
  case KEY_PAGE_DOWN:
    return Rml::Input::KI_NEXT;
  case KEY_SPACE:
    return Rml::Input::KI_SPACE;
  case KEY_LEFT_SHIFT:
    return Rml::Input::KI_LSHIFT;
  case KEY_RIGHT_SHIFT:
    return Rml::Input::KI_RSHIFT;
  case KEY_LEFT_CONTROL:
    return Rml::Input::KI_LCONTROL;
  case KEY_RIGHT_CONTROL:
    return Rml::Input::KI_RCONTROL;
  case KEY_LEFT_ALT:
    return Rml::Input::KI_LMENU;
  case KEY_RIGHT_ALT:
    return Rml::Input::KI_RMENU;
  default:
    return Rml::Input::KI_UNKNOWN;
  }
}

static int GetModifiers() {
  int m = 0;
  if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
    m |= Rml::Input::KM_SHIFT;
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    m |= Rml::Input::KM_CTRL;
  if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
    m |= Rml::Input::KM_ALT;
  return m;
}

// ---------------------------------------------------------------------------
// Forward raylib input events into an RmlUi context each frame
// ---------------------------------------------------------------------------
static void PumpInput(Rml::Context *ctx) {
  int mods = GetModifiers();

  // Mouse
  ctx->ProcessMouseMove(GetMouseX(), GetMouseY(), mods);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    ctx->ProcessMouseButtonDown(0, mods);
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    ctx->ProcessMouseButtonUp(0, mods);
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    ctx->ProcessMouseButtonDown(1, mods);
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    ctx->ProcessMouseButtonUp(1, mods);
  if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
    ctx->ProcessMouseButtonDown(2, mods);
  if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
    ctx->ProcessMouseButtonUp(2, mods);

  float wheel = GetMouseWheelMove();
  if (wheel != 0.0f)
    ctx->ProcessMouseWheel(wheel, mods);

  // Keyboard
  for (int key = GetKeyPressed(); key != 0; key = GetKeyPressed()) {
    auto rk = MapKey(key);
    if (rk != Rml::Input::KI_UNKNOWN)
      ctx->ProcessKeyDown(rk, mods);
  }

  // Text input (UTF-32 codepoints)
  for (int ch = GetCharPressed(); ch != 0; ch = GetCharPressed())
    ctx->ProcessTextInput(Rml::Character(ch));
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Nestled");

  // --- RmlUi init (OpenGL context is already live after InitWindow) ---
  RmlGL3::Initialize();

  RaylibSystemInterface sys_iface;
  RenderInterface_GL3 render_iface;
  Rml::SetSystemInterface(&sys_iface);
  Rml::SetRenderInterface(&render_iface);
  Rml::Initialise();

  render_iface.SetViewport(screenWidth, screenHeight);
  Rml::Context *ctx =
      Rml::CreateContext("main", Rml::Vector2i(screenWidth, screenHeight));
  Rml::Debugger::Initialise(ctx);

  // TODO: Load your fonts – RmlUi has no built-in default font.
  Rml::LoadFontFace("assets/fonts/Quicksand-Regular.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Light.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Bold.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Medium.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-SemiBold.ttf");

  // TODO: Load a UI document.
  if (auto *doc = ctx->LoadDocument("assets/ui/main_menu.rml"))
    doc->Show();

  bool showFps = false;

  // --- Main loop ---
  while (!WindowShouldClose()) {
    // Handle resize
    if (IsWindowResized()) {
      int w = GetScreenWidth(), h = GetScreenHeight();
      render_iface.SetViewport(w, h);
      ctx->SetDimensions(Rml::Vector2i(w, h));
    }

    if (IsKeyPressed(KEY_FOUR))
      showFps = !showFps;
    if (IsKeyPressed(KEY_SIX))
      Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
    if (IsKeyPressed(KEY_FIVE)) {
        if (IsWindowState(FLAG_VSYNC_HINT))
            ClearWindowState(FLAG_VSYNC_HINT);
        else
            SetWindowState(FLAG_VSYNC_HINT);
    }

    PumpInput(ctx);
    ctx->Update();

    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (showFps) DrawFPS(10, 10);

    // ---- Draw your game with raylib here ----
    //DrawText("Nestled — raylib + RmlUi", 10, 10, 20, DARKGRAY);
    //DrawCircle(screenWidth / 2, screenHeight / 2, 40.0f, MAROON);

    // Flush raylib's batch, then let RmlUi render with raw OpenGL
    rlDrawRenderBatchActive();
    render_iface.BeginFrame();
    ctx->Render();
    render_iface.EndFrame();

    EndDrawing();
  }

  // --- Cleanup ---
  Rml::Shutdown();
  RmlGL3::Shutdown();
  CloseWindow();

  return 0;
}
