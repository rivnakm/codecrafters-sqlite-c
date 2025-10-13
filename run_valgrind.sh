#!/usr/bin/env bash

# Build
just build debug

# Run Valgrind on the executable
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-error-list=yes ./build/sqlite "$@"
