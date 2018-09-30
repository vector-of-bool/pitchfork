set(_pf_url http://localhost:8000/pf-cmake)
if(DEFINED PF_URL_BASE)
    set(_pf_url "${PF_URL_BASE}")
endif()
set(PF_URL_BASE "${_pf_url}" CACHE STRING "Base URL to download Pitchfork files")
set(_PF_BASEDIR "${CMAKE_BINARY_DIR}/_pf" CACHE PATH "Directory for Pitchfork files")

if(NOT DEFINED PF_VERSION)
    set(PF_VERSION 0.1.0)
endif()

set(_PF_DIR "${_PF_BASEDIR}/${PF_VERSION}" CACHE INTERNAL "Directory for Pitchfork files")
set(_PF_ENTRY_FILE "${_PF_DIR}/entry.cmake")
set(PF_URL "${PF_URL_BASE}/${PF_VERSION}" CACHE STRING "URL for Pitchfork at requested version")

if(NOT EXISTS "${_PF_ENTRY_FILE}" OR TRUE)
    file(
        DOWNLOAD "${PF_URL}/entry.cmake"
        "${_PF_ENTRY_FILE}.tmp"
        STATUS pair
        )
    list(GET pair 0 rc)
    list(GET pair 1 msg)
    if(NOT rc EQUAL 0)
        file(REMOVE "${_PF_ENTRY_FILE}")
        message(FATAL_ERROR "Failed to download Pitchfork modules [${rc}]: ${msg}")
    endif()
    file(RENAME "${_PF_ENTRY_FILE}.tmp" "${_PF_ENTRY_FILE}")
endif()

include("${_PF_ENTRY_FILE}")
