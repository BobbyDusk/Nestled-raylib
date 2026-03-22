cmake_minimum_required(VERSION 3.14)
project(Nestled LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# All dependencies built as static libraries for a small single binary.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# --- RAYLIB (2D-only, stripped down) ---
set(CUSTOMIZE_BUILD        ON  CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES         OFF CACHE BOOL "" FORCE)
set(SUPPORT_MODULE_RMODELS OFF CACHE BOOL "" FORCE) # No 3D models
set(SUPPORT_CAMERA_SYSTEM  OFF CACHE BOOL "" FORCE) # No 3D camera
set(SUPPORT_SCREEN_CAPTURE OFF CACHE BOOL "" FORCE)
set(SUPPORT_GIF_RECORDING  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# --- RMLUI ---
set(RMLUI_SAMPLES          OFF CACHE BOOL "" FORCE)
set(RMLUI_COMPILER_OPTIONS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    rmlui
    GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
    GIT_TAG 6.2
)
FetchContent_MakeAvailable(rmlui)

# Compile RmlUi's GL3 renderer from source (works with raylib's OpenGL context).
# Use raylib's glad to avoid duplicate GL loader symbols.
set(RMLUI_BACKEND_DIR "${rmlui_SOURCE_DIR}/Backends")
add_library(rmlui_renderer_gl3 STATIC
    "${RMLUI_BACKEND_DIR}/RmlUi_Renderer_GL3.cpp"
)
target_include_directories(rmlui_renderer_gl3 PUBLIC "${RMLUI_BACKEND_DIR}")
target_include_directories(rmlui_renderer_gl3 PRIVATE "${raylib_SOURCE_DIR}/src/external")
target_compile_definitions(rmlui_renderer_gl3 PRIVATE "RMLUI_GL3_CUSTOM_LOADER=<glad.h>")
target_link_libraries(rmlui_renderer_gl3 PUBLIC rmlui_core)

# --- GAME ---
add_executable(${PROJECT_NAME} src/main.cpp src/RmlUiLayer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib rmlui rmlui_renderer_gl3)
target_compile_options(${PROJECT_NAME} PRIVATE -Os)
