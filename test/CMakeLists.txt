include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# project vars for testings
set(UNIT_TEST_SOLITARY_EXE_NAME unit_tests_solitary)
set(UNIT_TEST_SOCIABLE_EXE_NAME unit_tests_sociable)
set(INTEGRATION_TEST_EXE_NAME integration_tests)

add_subdirectory(unit)
add_subdirectory(integration)