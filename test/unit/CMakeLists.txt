add_executable(
  ${UNIT_TEST_SOLITARY_EXE_NAME}
)

add_executable(
  ${UNIT_TEST_SOCIABLE_EXE_NAME}
)

target_link_libraries(
  ${UNIT_TEST_SOLITARY_EXE_NAME}
  PUBLIC ${LIBRARY_NAME_STATIC}
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

target_link_libraries(
  ${UNIT_TEST_SOCIABLE_EXE_NAME}
  PUBLIC ${LIBRARY_NAME_STATIC}
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

add_subdirectory(sociable)
add_subdirectory(solitary)

include(GoogleTest)
gtest_discover_tests(${UNIT_TEST_SOCIABLE_EXE_NAME})
gtest_discover_tests(${UNIT_TEST_SOLITARY_EXE_NAME})