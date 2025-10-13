format:
    clang-format -i --verbose $(fd '.*\.[ch]' .)

build preset="debug":
    cmake --preset={{ preset }} .
    ninja -C build

test: build
    ninja -C build test
