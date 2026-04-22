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

  Rml::LoadFontFace("assets/fonts/Mansalva-Regular.ttf");

  if (auto *doc = rml.GetContext()->LoadDocument("assets/ui/main_menu.rml")) {
    doc->Show();

    // For each ui-elements subfolder, apply random decorator images to all
    // elements whose class name matches the folder name.
    std::mt19937 rng(std::random_device{}());
    for (const auto &uiElemEntry : std::filesystem::directory_iterator("assets/ui-elements")) {
      if (!uiElemEntry.is_directory())
        continue;

      std::string className = uiElemEntry.path().filename().string();

      std::vector<std::string> files;
      for (const auto &entry : std::filesystem::directory_iterator(uiElemEntry.path()))
        if (entry.is_regular_file())
          files.push_back("../ui-elements/" + className + "/" + entry.path().filename().string());

      if (files.empty())
        continue;

      Rml::ElementList els;
      doc->GetElementsByClassName(els, className);
      size_t prev = files.size(); // sentinel: no previous index
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

    // Run one update so RmlUi resolves styles for elements whose decorator
    // was just set via SetProperty, suppressing spurious font warnings.
    rml.GetContext()->Update();
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
