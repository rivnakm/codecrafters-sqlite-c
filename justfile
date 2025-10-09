format:
    clang-format -i --verbose $(fd '.*\.[ch]' .)

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug .
    ninja -C build

test:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=True .
    ninja -C build
    ninja -C build test
    