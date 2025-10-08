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

set(RUN_CLANG_TIDY OFF)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# clang tidy setup
if (RUN_CLANG_TIDY)
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")

        # Try to find .clang-tidy config file in project root
        find_file(CLANG_TIDY_CONFIG_FILE
            NAMES ".clang-tidy"
            PATHS "${CMAKE_SOURCE_DIR}"
            NO_DEFAULT_PATH
        )

        set(EXTRA_CLANG_TIDY_ARGS "")

        list(APPEND EXTRA_CLANG_TIDY_ARGS
        "--extra-arg=-I${CMAKE_SOURCE_DIR}/lib"
        "--extra-arg=-I${CMAKE_SOURCE_DIR}/lib/commands"
        "--extra-arg=-I${CMAKE_SOURCE_DIR}/lib/parsing"
        "--extra-arg=-I${CMAKE_SOURCE_DIR}/lib/logging"
        )
        string(JOIN ";" EXTRA_CLANG_TIDY_ARGS_STR ${EXTRA_CLANG_TIDY_ARGS})

        if(CLANG_TIDY_CONFIG_FILE)
            message(STATUS "Using clang-tidy config: ${CLANG_TIDY_CONFIG_FILE}")
        else()
            message(WARNING "No .clang-tidy config found in ${CMAKE_SOURCE_DIR}")
            set(CLANG_TIDY_CONFIG_FILE "")
        endif()

        # Apply clang-tidy checks automatically during build
        set(CMAKE_CXX_CLANG_TIDY
            "${CLANG_TIDY_EXE};--config-file=${CLANG_TIDY_CONFIG_FILE};-p=${CMAKE_SOURCE_DIR}/build;${EXTRA_CLANG_TIDY_ARGS_STR}"
        )
    else()
        message("clang-tidy not found — skipping static analysis setup")
    endif()
endif()

# clang format setup
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_TIDY_EXE}")

    set(CLANG_FORMAT_FILE "${CMAKE_SOURCE_DIR}/.clang-format")
    if(NOT EXISTS "${CLANG_FORMAT_FILE}")
        message(FATAL_ERROR ".clang-format not found in ${CMAKE_SOURCE_DIR}")
    endif()

    file(GLOB_RECURSE ALL_SRC
        "${CMAKE_SOURCE_DIR}/lib/*.cpp"
        "${CMAKE_SOURCE_DIR}/lib/*.h"
        "${CMAKE_SOURCE_DIR}/test/*.cpp"
        "${CMAKE_SOURCE_DIR}/test/*.h"
        "${CMAKE_SOURCE_DIR}/tools/*.cpp"
        "${CMAKE_SOURCE_DIR}/tools/*.h"
    )

    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SRC}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format on source files..."
    )
endif()

option(ENABLE_COVERAGE "Enable GCC/MinGW coverage" ON)
# Make BUILD_SHARED_LIBS visible in GUIs like ccmake / cmake-gui
option(BUILD_SHARED_LIBS "Build libraries as shared instead of static" OFF)
option(ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Enable warnings-as-errors depending on compiler
if(ENABLE_WARNINGS_AS_ERRORS)
    if(MSVC)
        # Microsoft Visual C++
        add_compile_options(/W4 /WX /EHsc)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /WX /EHsc")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /WX /EHsc")
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
option(BUILD_DEMO "Build demo folder" OFF)
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