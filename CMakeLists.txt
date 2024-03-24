cmake_minimum_required(VERSION 3.18)

# TODO add msvc support
project(flashman LANGUAGES C)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI hidapi-hidraw)

add_executable(flashman
    "src/main.c"
    "src/db.h"
    "src/flashboy.c"
    "src/flashboy.h"
)

target_include_directories(flashman PRIVATE
    "src"
    ${HIDAPI_INCLUDE_DIRS}
)

target_link_libraries(flashman PRIVATE ${HIDAPI_LINK_LIBRARIES})

# compiler warnings
target_compile_options(flashman PRIVATE -Wall -Wvla -Wformat=2 ${HIDAPI_CFLAGS_OTHER})

# language versions
set_target_properties(flashman PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
