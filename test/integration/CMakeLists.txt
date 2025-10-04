add_executable(
  ${INTEGRATION_TEST_EXE_NAME}
)

target_link_libraries(
  ${INTEGRATION_TEST_EXE_NAME}
  PUBLIC ${LIBRARY_NAME_STATIC}
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

add_subdirectory(logging)

include(GoogleTest)
gtest_discover_tests(${INTEGRATION_TEST_EXE_NAME})