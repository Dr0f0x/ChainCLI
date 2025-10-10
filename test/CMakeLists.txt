include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Disable clang-tidy for GoogleTest targets
if(TARGET gtest)
    set_target_properties(gtest PROPERTIES CXX_CLANG_TIDY "")
endif()
if(TARGET gtest_main)
    set_target_properties(gtest_main PROPERTIES CXX_CLANG_TIDY "")
endif()
if(TARGET gmock)
    set_target_properties(gmock PROPERTIES CXX_CLANG_TIDY "")
endif()
if(TARGET gmock_main)
    set_target_properties(gmock_main PROPERTIES CXX_CLANG_TIDY "")
endif()

# project vars for testings
set(UNIT_TEST_SOLITARY_EXE_NAME unit_tests_solitary)
set(UNIT_TEST_SOCIABLE_EXE_NAME unit_tests_sociable)
set(INTEGRATION_TEST_EXE_NAME integration_tests)

set_source_files_properties(gtest_main.cc.o PROPERTIES
    SKIP_LINTING ON
)

add_subdirectory(unit)
add_subdirectory(integration)