#include "raylib.h"

#include "RmlUiLayer.h"
#include <RmlUi/Core.h>

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main() {
  const int screenWidth  = 1280;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Nestled");

  RmlUiLayer rml;
  rml.Init(screenWidth, screenHeight);

  Rml::LoadFontFace("assets/fonts/Quicksand-Regular.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Light.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Bold.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Medium.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-SemiBold.ttf");

  if (auto *doc = rml.GetContext()->LoadDocument("assets/ui/main_menu.rml"))
    doc->Show();

  bool showFps = false;

  // --- Main loop ---
  while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
    if (IsWindowResized())
      rml.OnResize(GetScreenWidth(), GetScreenHeight());

    if (IsKeyPressed(KEY_FOUR)) showFps = !showFps;
    if (IsKeyPressed(KEY_SIX))  rml.ToggleDebugger();
    if (IsKeyPressed(KEY_FIVE)) {
      if (IsWindowState(FLAG_VSYNC_HINT)) ClearWindowState(FLAG_VSYNC_HINT);
      else                                SetWindowState(FLAG_VSYNC_HINT);
    }

    rml.PumpInput();
    rml.Update();

    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (showFps) DrawFPS(10, 10);

    // ---- Draw your game with raylib here ----

    rml.Render();
    EndDrawing();
  }

  rml.Shutdown();
  CloseWindow();

  return 0;
}
