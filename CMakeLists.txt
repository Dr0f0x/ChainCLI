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

# project specific vars
set(LIBRARY_NAME_STATIC cli_lib_static)
set(LIBRARY_NAME_SHARED cli_lib)
set(DEMO_OUTPUT_NAME cli_demo)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    #set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}") //TODO currently produces errors
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

option(BUILD_TOOLS "Build tools folder" ON)
option(BUILD_LIB "Build library folder" ON)
option(BUILD_DEMO "Build demo folder" ON)
option(BUILD_TESTS "Build tests folder" ON)

enable_testing()
add_subdirectory(tools)

# Build the library
if(BUILD_LIB)
    add_subdirectory(lib)
endif()

# Include tests
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# Build the demo app
if(BUILD_DEMO)
    add_subdirectory(demo)
endif()