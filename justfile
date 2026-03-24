default_target := "build"
build_dir := "build"
compiler_c := "clang"
compiler_cxx := "clang++"
generator := "Ninja"

# Configure the CMake project with AddressSanitizer (default for development)
configure build_type="Debug":
    cmake -B {{build_dir}} -S . \
        -G {{generator}} \
        -DCMAKE_BUILD_TYPE={{build_type}} \
        -DCMAKE_C_COMPILER={{compiler_c}} \
        -DCMAKE_CXX_COMPILER={{compiler_cxx}} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g -fsanitize-ignorelist={{justfile_directory()}}/asan_ignorelist.txt" \
        -DCMAKE_C_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g -fsanitize-ignorelist={{justfile_directory()}}/asan_ignorelist.txt" \
        -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address -fsanitize=undefined"

# Configure without sanitizers (for release or performance testing)
configure-noasan build_type="Debug":
    cmake -B {{build_dir}} -S . \
        -G {{generator}} \
        -DCMAKE_BUILD_TYPE={{build_type}} \
        -DCMAKE_C_COMPILER={{compiler_c}} \
        -DCMAKE_CXX_COMPILER={{compiler_cxx}} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
build *args="":
    cmake --build {{build_dir}} -j$(nproc) {{args}}

# Configure as MinSizeRel without sanitizers and build
release:
    just configure-noasan MinSizeRel
    just build

# Run the game with AddressSanitizer (default, from project root so asset paths resolve)
run: build
    ASAN_OPTIONS=detect_leaks=1:fast_unwind_on_malloc=0 ./{{build_dir}}/Nestled

# Run without sanitizers (configure without ASan first, then run)
run-noasan:
    just configure-noasan
    just build
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

# Run with Valgrind (memcheck for leak detection)
valgrind: build
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
        --verbose --log-file=valgrind-out.txt ./{{build_dir}}/Nestled

# Run with Valgrind (interactive, output to terminal)
valgrind-term: build
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
        ./{{build_dir}}/Nestled
