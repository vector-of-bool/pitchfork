file(GLOB pyenv_versions "$ENV{HOME}/.pyenv/versions/*")

set(_prev "${CONAN_EXECUTABLE}")
find_program(
    CONAN_EXECUTABLE conan
    PATHS
        ${pyenv_versions}
        "$ENV{HOME}/.local"
    PATH_SUFFIXES
        bin
        Scripts
    DOC "Path to Conan executable"
    )
if(CONAN_EXECUTABLE)
    if(NOT _prev)
        message(STATUS "Found Conan: ${CONAN_EXECUTABLE}")
    endif()
    set(Conan_FOUND TRUE CACHE INTERNAL "")
else()
    set(Conan_FOUND FALSE CACHE INTERNAL "")
    if(Conan_FIND_REQUIRED)
        message(FATAL_ERROR "Did not find a Conan executable")
    endif()
endif()

function(_conan_install)
    set(src "${CMAKE_CURRENT_SOURCE_DIR}")
    set(bin "${CMAKE_CURRENT_BINARY_DIR}")
    get_filename_component(conanfile conanfile.txt ABSOLUTE)
    get_filename_component(conan_paths "${bin}/conan_paths.cmake" ABSOLUTE)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${conanfile}")

    if("${conanfile}" IS_NEWER_THAN "${conan_paths}")
        message(STATUS "Installing Conan requirements from ${conanfile}")
        execute_process(
            COMMAND "${CONAN_EXECUTABLE}" install "${src}"
                -e "C=${CMAKE_C_COMPILER}"
                -e "CXX=${CMAKE_CXX_COMPILER}"
                ${ARGN}
            WORKING_DIRECTORY "${bin}"
            RESULT_VARIABLE retc
            )
        if(retc)
            message(SEND_ERROR "Conan install failed [${retc}]:\n${out}")
        endif()
    endif()
    set(__conan_paths "${conan_paths}" PARENT_SCOPE)
endfunction()

macro(conan_install)
    _conan_install(${ARGN})
    include("${__conan_paths}" OPTIONAL RESULT_VARIABLE __was_included)
    if(NOT __was_included)
        message(SEND_ERROR "Conan dependencies were not imported (Expected file ${__conan_paths}). You may need to run Conan manually (from the build directory).")
    endif()
    unset(__conan_paths)
    unset(__was_included)
endmacro()
