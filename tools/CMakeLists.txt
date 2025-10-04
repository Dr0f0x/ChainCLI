add_subdirectory(heady)

add_custom_target(
    run_heady
    COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/run_heady.ps1
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running Heady amalgamation script"
)