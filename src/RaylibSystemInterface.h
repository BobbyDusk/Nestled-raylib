#pragma once

#include "raylib.h"
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/Types.h>

class RaylibSystemInterface : public Rml::SystemInterface {
public:
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
};
