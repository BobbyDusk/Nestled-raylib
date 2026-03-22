default_target := "build"
build_dir := "build"
compiler_c := "clang"
compiler_cxx := "clang++"

# Configure the CMake project (run once or after CMakeLists.txt changes)
configure build_type="Debug":
    cmake -B {{build_dir}} -S . \
        -DCMAKE_BUILD_TYPE={{build_type}} \
        -DCMAKE_C_COMPILER={{compiler_c}} \
        -DCMAKE_CXX_COMPILER={{compiler_cxx}} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
build *args="":
    cmake --build {{build_dir}} -j$(nproc) {{args}}

# Configure as MinSizeRel and build
release:
    just configure MinSizeRel
    just build

# Run the game (from project root so asset paths resolve)
run: build
    ./{{build_dir}}/Nestled

# Clean build artifacts
clean:
    rm -rf {{build_dir}}

# Full rebuild from scratch
rebuild:
    just clean
    just configure
    just build

# Show stripped binary size
size: build
    strip {{build_dir}}/Nestled -o /tmp/Nestled_stripped
    ls -lh {{build_dir}}/Nestled /tmp/Nestled_stripped
