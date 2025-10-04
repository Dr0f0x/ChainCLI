cmake_minimum_required(VERSION 3.25.0)
project(
    CliLib
    VERSION 1.0
    DESCRIPTION "CLI Library"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# prohect specific vars
set(LIBRARY_NAME_STATIC cli_lib_static)
set(LIBRARY_NAME_SHARED cli_lib)
set(DEMO_OUTPUT_NAME cli_demo)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    #set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
else()
    message(STATUS "clang-tidy not found")
endif()

option(ENABLE_COVERAGE "Enable GCC/MinGW coverage" ON)
# Make BUILD_SHARED_LIBS visible in GUIs like ccmake / cmake-gui
option(BUILD_SHARED_LIBS "Build libraries as shared instead of static" OFF)
option(ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Enable warnings-as-errors depending on compiler
if(ENABLE_WARNINGS_AS_ERRORS)
    if(MSVC)
        # Microsoft Visual C++
        add_compile_options(/W4 /WX)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang and AppleClang
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

enable_testing()

# Build the library
add_subdirectory(lib)

# Build the demo app
add_subdirectory(demo)

# Include tests
add_subdirectory(test)

# Include tools
add_subdirectory(tools)