#include "raylib.h"

#include "RmlUiLayer.h"
#include <RmlUi/Core.h>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main() {
  const int screenWidth = 1280;
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
  Rml::LoadFontFace("assets/fonts/Caveat-Regular.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Bold.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-Medium.ttf");
  Rml::LoadFontFace("assets/fonts/Quicksand-SemiBold.ttf");


  if (auto *doc = rml.GetContext()->LoadDocument("assets/ui/main_menu.rml")) {
    doc->Show();

    // Pick a concrete random file per button so each gets a unique path
    // and RmlUi's texture cache doesn't collapse them into one texture.
    std::vector<std::string> files;
    for (const auto &entry : std::filesystem::directory_iterator("assets/images/text_button"))
      if (entry.is_regular_file())
        files.push_back("../images/text_button/" + entry.path().filename().string());

    if (!files.empty()) {
      std::mt19937 rng(std::random_device{}());
      Rml::ElementList els;
      doc->GetElementsByClassName(els, "text-button");
      size_t prev = files.size(); // sentinel: no previous
      for (auto *el : els) {
        // Build a pool excluding the previously used index.
        std::vector<size_t> pool;
        pool.reserve(files.size());
        for (size_t i = 0; i < files.size(); i++)
          if (i != prev)
            pool.push_back(i);
        // Fall back to full pool if only one image exists.
        if (pool.empty())
          pool.push_back(prev);
        prev = pool[std::uniform_int_distribution<size_t>(0, pool.size() - 1)(rng)];
        el->SetProperty("decorator", "image(" + files[prev] + " fill)");
      }
    }
  }

  bool showFps = false;

  // --- Main loop ---
  while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
    if (IsWindowResized())
      rml.OnResize(GetScreenWidth(), GetScreenHeight());

    if (IsKeyPressed(KEY_FOUR))
      showFps = !showFps;
    if (IsKeyPressed(KEY_SIX))
      rml.ToggleDebugger();
    if (IsKeyPressed(KEY_FIVE)) {
      if (IsWindowState(FLAG_VSYNC_HINT))
        ClearWindowState(FLAG_VSYNC_HINT);
      else
        SetWindowState(FLAG_VSYNC_HINT);
    }

    rml.PumpInput();
    rml.Update();

    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (showFps)
      DrawFPS(10, 10);

    // ---- Draw your game with raylib here ----

    rml.Render();
    EndDrawing();
  }

  rml.Shutdown();
  CloseWindow();

  return 0;
}
