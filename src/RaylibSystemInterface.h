#pragma once

#include "raylib.h"
#include "GLFW/glfw3.h"
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/Types.h>
#include <unordered_map>
#include <string>

class RaylibSystemInterface : public Rml::SystemInterface {
public:
    // Call after InitWindow(), before RmlUi init.
    // Expects PNG files; falls back to OS default if a file is missing.
    void LoadCursors() {
        // Custom image cursors — hotspot is the pixel that maps to the click point
        LoadImageCursor("default", "assets/images/cursor/default.png",  8,  8);
        LoadImageCursor("arrow",   "assets/images/cursor/default.png",  8,  8);
        LoadImageCursor("pointer", "assets/images/cursor/pointer.png",  8,  8);
        LoadImageCursor("text",    "assets/images/cursor/text.png",     6, 12);

        // Standard OS cursors for the remaining RmlUi cursor names
        cursors_["move"]        = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        cursors_["resize"]      = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        cursors_["cross"]       = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        cursors_["unavailable"] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
    }

    // Call from RmlUiLayer::Shutdown(), before CloseWindow().
    void DestroyCursors() {
        for (auto& [name, cursor] : cursors_)
            glfwDestroyCursor(cursor);
        cursors_.clear();
    }

    double GetElapsedTime() override { return GetTime(); }

    bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
        int level = LOG_INFO;
        switch (type) {
        case Rml::Log::LT_ERROR:   level = LOG_ERROR;   break;
        case Rml::Log::LT_WARNING: level = LOG_WARNING; break;
        case Rml::Log::LT_INFO:    level = LOG_INFO;    break;
        case Rml::Log::LT_DEBUG:   level = LOG_DEBUG;   break;
        default: break;
        }
        TraceLog(level, "RmlUi: %s", message.c_str());
        return true;
    }

    void SetClipboardText(const Rml::String& text) override {
        ::SetClipboardText(text.c_str());
    }

    void GetClipboardText(Rml::String& text) override {
        text = ::GetClipboardText();
    }

    void SetMouseCursor(const Rml::String& cursor_name) override {
        GLFWwindow* window = (GLFWwindow*)GetWindowHandle();
        auto it = cursors_.find(std::string(cursor_name));
        // nullptr resets to the OS default cursor
        glfwSetCursor(window, it != cursors_.end() ? it->second : nullptr);
    }

private:
    std::unordered_map<std::string, GLFWcursor*> cursors_;

    void LoadImageCursor(const char* name, const char* path, int hotspot_x, int hotspot_y) {
        Image img = LoadImage(path);
        if (img.data == nullptr) return;
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        GLFWimage glfwImg = { img.width, img.height, (unsigned char*)img.data };
        GLFWcursor* cursor = glfwCreateCursor(&glfwImg, hotspot_x, hotspot_y);
        if (cursor) cursors_[name] = cursor;
        UnloadImage(img);
    }
};
