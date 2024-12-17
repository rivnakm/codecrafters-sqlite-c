cmake_minimum_required(VERSION 3.13)
project(codecrafters-sqlite LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTS "Build unit tests" FALSE)

add_library(sqlite-lib STATIC
  lib/src/bytes.c
  lib/src/cell.c
  lib/src/file_header.c
  lib/src/page_header.c
  lib/src/record.c
  lib/src/varint.c
)
target_include_directories(sqlite-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/)
set_target_properties(sqlite-lib PROPERTIES OUTPUT_NAME "cc-sqlite") # CodeCrafters SQLite

add_executable(sqlite
  src/main.c
)
target_link_libraries(sqlite PRIVATE sqlite-lib)

if (BUILD_TESTS)
  include(FetchContent)

  FetchContent_Declare(
    munit
    GIT_REPOSITORY https://github.com/nemequ/munit.git
  )
  FetchContent_MakeAvailable(munit)

  add_library(munit STATIC ${munit_SOURCE_DIR}/munit.c)
  target_include_directories(munit PUBLIC ${munit_SOURCE_DIR})

  add_executable(tests 
    tests/test.c
    tests/bytes_test.c
    tests/file_header_test.c
    tests/varint_test.c
  )
  target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/)
  target_link_libraries(tests PRIVATE sqlite-lib munit)
  target_compile_definitions(tests PUBLIC EXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/examples")

  add_custom_target(
    test
    COMMAND tests
    DEPENDS tests
  )
endif()
