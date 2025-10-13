format:
    clang-format -i --verbose $(fd '.*\.[ch]' .)

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=True, -DENABLE_UBSAN=True -DBUILD_TESTS=True .
    ninja -C build

test: build
    ninja -C build test
