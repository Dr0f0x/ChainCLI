# Create main library combining all modules
add_library(${LIBRARY_OUTPUT_NAME}
    cli_base.cpp
    cli_base.h
    cli_config.h
    cli_base.h
    utils.cpp
    utils.h
)

# Add subdirectories for modules
add_subdirectory(parsing)
add_subdirectory(commands)
add_subdirectory(logging)


#TODO hide internal api
# Define BUILD_MY_DLL when building the library
#target_compile_definitions(${LIBRARY_OUTPUT_NAME} PRIVATE BUILD_MY_DLL)

# Hide symbols by default on GCC/Clang (only works with dll)
#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(${LIBRARY_OUTPUT_NAME} PRIVATE -fvisibility=hidden)
#endif()

# Include path for consumers
target_include_directories(${LIBRARY_OUTPUT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)