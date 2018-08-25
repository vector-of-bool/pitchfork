option(BUILD_VSCE "Build the Pitchfork VSCode extension" ON)
if(NOT YARN_EXECUTABLE)
    message(STATUS "No yarn executable. Not building VSCode extension.")
elseif(BUILD_VSCE)
    get_filename_component(stamp "${CMAKE_CURRENT_BINARY_DIR}/vsce.stamp" ABSOLUTE)
    get_filename_component(vsce_root vscode-pitchfork ABSOLUTE)
    file(GLOB_RECURSE vsce_sources LIST_DIRECTORIES false CONFIGURE_DEPENDS ${vsce_root}/**)
    add_custom_command(
        OUTPUT "${stamp}"
        SOURCES ${vsce_sources}
        COMMAND "${YARN_EXECUTABLE}"
        COMMAND "${YARN_EXECUTABLE}" compile
        COMMAND "${CMAKE_COMMAND}" -E touch "${stamp}"
        COMMENT "Building VSCode extension"
        WORKING_DIRECTORY "${vsce_root}"
        )
    add_custom_target(pf-vsce DEPENDS "${stamp}")
endif()
