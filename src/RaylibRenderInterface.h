#pragma once

#include "RmlUi_Renderer_GL3.h"
#include "raylib.h"
#include <filesystem>
#include <random>
#include <vector>

// Extends the RmlUI GL3 renderer to load textures via raylib's LoadImage,
// supporting PNG, JPG, BMP, TGA, and any other format raylib handles.
class RaylibRenderInterface : public RenderInterface_GL3 {
public:
    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override {
        std::filesystem::path path(source.c_str());

        if (std::filesystem::is_directory(path)) {
            std::vector<std::filesystem::path> files;
            for (const auto& entry : std::filesystem::directory_iterator(path))
                if (entry.is_regular_file())
                    files.push_back(entry.path());

            if (files.empty())
                return {};

            std::mt19937 rng(std::random_device{}());
            path = files[std::uniform_int_distribution<size_t>(0, files.size() - 1)(rng)];
        }

        Image img = LoadImage(path.c_str());
        if (img.data == nullptr)
            return {};

        // Normalize to 32-bit RGBA, then premultiply alpha (consistent with
        // the GL3 backend's TGA loader and RmlUI's expected blend mode).
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        ImageAlphaPremultiply(&img);

        texture_dimensions.x = img.width;
        texture_dimensions.y = img.height;

        const size_t data_size = static_cast<size_t>(img.width * img.height * 4);
        Rml::TextureHandle handle = GenerateTexture(
            {reinterpret_cast<const Rml::byte*>(img.data), data_size},
            texture_dimensions
        );

        UnloadImage(img);
        return handle;
    }
};
