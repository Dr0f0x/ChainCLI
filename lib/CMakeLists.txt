# Create main library combining all modules
add_library(${LIBRARY_NAME_STATIC} STATIC
    cli_app.cpp
    cli_app.h
    cli_config.h
    cli_context.h
    cli_context.cpp
)

add_library(${LIBRARY_NAME_SHARED} SHARED
    cli_app.cpp
    cli_config.h
    cli_app.h
    cli_context.h
    cli_context.cpp
)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Coverage build enabled (MinGW/GCC).")
        target_compile_options(${LIBRARY_NAME_STATIC} PUBLIC --coverage)
        target_link_options(${LIBRARY_NAME_STATIC} PUBLIC --coverage)
        target_compile_options(${LIBRARY_NAME_SHARED} PUBLIC --coverage)
        target_link_options(${LIBRARY_NAME_SHARED} PUBLIC --coverage)
    else()
        message(WARNING "Coverage only supported with GCC/MinGW on Windows.")
    endif()
endif()

# Add subdirectories for modules
add_subdirectory(parsing)
add_subdirectory(commands)
add_subdirectory(logging)

if(MSVC)
    target_compile_options(${LIBRARY_NAME_STATIC} PRIVATE "/utf-8")
endif()


#TODO hide internal api
# Define BUILD_MY_DLL when building the library
#target_compile_definitions(${LIBRARY_NAME_STATIC} PRIVATE BUILD_MY_DLL)

# Hide symbols by default on GCC/Clang (only works with dll)
#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(${LIBRARY_NAME_STATIC} PRIVATE -fvisibility=hidden)
#endif()

# Include path for consumers
target_include_directories(${LIBRARY_NAME_STATIC}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(${LIBRARY_NAME_SHARED}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)