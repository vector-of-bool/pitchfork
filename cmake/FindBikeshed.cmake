include(CMakeFindDependencyMacro)

find_dependency(Python2 COMPONENTS Interpreter)

macro(_fail_find reason)
    set(Bikeshed_FOUND FALSE CACHE BOOL "Was Bikeshed found?" FORCE)
    if(Bikeshed_FIND_REQUIRED)
        message(FATAL_ERROR "${reason}")
    endif()
    return()
endmacro()

if(NOT Python2_FOUND)
    _fail_find("Failed to find bikeshed: We require a Python 2 executable.")
endif()

get_target_property(py2 Python2::Interpreter LOCATION)

if(NOT DEFINED PF_VIRTUALENV_MODULE)
    message(STATUS "Finding Python virtualenv module...")
endif()
if(NOT PF_VIRTUALENV_MODULE)
    set(found)
    foreach(try_mod IN ITEMS venv virtualenv NOTFOUND)
        execute_process(
            COMMAND "${py2}" -m "${try_mod}" --help
            OUTPUT_VARIABLE out
            ERROR_VARIABLE out
            RESULT_VARIABLE retc
            )
        if(retc EQUAL 0)
            message(STATUS "Found virtualenv module: ${try_mod}")
            set(found "${try_mod}")
            break()
        endif()
    endforeach()
    if(found AND NOT PF_VIRTUALENV_MODULE)
        # We've found the mod after failing to find it. Clear the cached failure.
        unset(PF_VIRTUALENV_MODULE CACHE)
    endif()
    set(PF_VIRTUALENV_MODULE "${found}" CACHE STRING "Python 2 module containing virtualenv")
endif()

if(NOT PF_VIRTUALENV_MODULE)
    message(STATUS "${PF_VIRTUALENV_MODULE}")
    _fail_find("Failed to find Bikeshed: Python 2 installation must contain virtualenv")
endif()

get_filename_component(bs_venv_dir "${CMAKE_BINARY_DIR}/_bikeshed_venv" ABSOLUTE)
get_filename_component(bs_stamp "${bs_venv_dir}/.stamp" ABSOLUTE)

macro(_bikeshed_found)
    set(Bikeshed_FOUND TRUE CACHE BOOL "Was Bikeshed found?" FORCE)
    file(TOUCH "${bs_stamp}")
    find_program(
        _bs_exe
        NAMES bikeshed bikeshed.exe
        PATH_SUFFIXES Scripts bin
        NO_DEFAULT_PATH
        PATHS "${bs_venv_dir}"
        )
    add_executable(Bikeshed::Bikeshed IMPORTED)
    set_target_properties(Bikeshed::Bikeshed PROPERTIES IMPORTED_LOCATION "${_bs_exe}")
    # Check if the bikeshed data needs to be updated
    execute_process(
        COMMAND "${_bs_exe}" -d update
        OUTPUT_VARIABLE out
        )
    if(NOT out MATCHES "already up-to-date")
        message(STATUS "Updating Bikeshed local data...")
        execute_process(COMMAND "${_bs_exe}" --silent update)
    endif()
    unset(_bs_exe CACHE)
    return()
endmacro()

if(EXISTS "${bs_stamp}")
    _bikeshed_found()
endif()

# Download the latest Bikeshed from master
include(FetchContent)
FetchContent_Declare(
    bikeshed
    URL https://github.com/tabatkins/bikeshed/archive/master.zip
    )
FetchContent_GetProperties(bikeshed)
if(NOT bikeshed_POPULATED)
    message(STATUS "Obtaining Bikeshed... (This may take a moment)")
    FetchContent_Populate(bikeshed)
    message(STATUS "Bikeshed is downloaded")
endif()

file(REMOVE_RECURSE "${bs_venv_dir}")

message(STATUS "Creating virtualenv for Bikeshed...")
execute_process(
    COMMAND "${py2}" -m "${PF_VIRTUALENV_MODULE}" "${bs_venv_dir}"
    OUTPUT_VARIABLE out
    ERROR_VARIABLE out
    RESULT_VARIABLE retc
    )

if(retc)
    message(WARNING "Failed to create virtualenv:\n${out}")
    _fail_find("Could not create virtualenv")
endif()

get_filename_component(py_filename "${py2}" NAME)
find_program(_venv_py2
    NAMES ${py_filename} python2.7 python2 python
    PATH_SUFFIXES Scripts bin
    NO_DEFAULT_PATH
    PATHS "${bs_venv_dir}"
    )
set(venv_py2 "${_venv_py2}")
unset(_venv_py2 CACHE)

message(STATUS "Installing Bikeshed in local virtualenv")
execute_process(
    COMMAND "${venv_py2}" -m pip install --editable "${bikeshed_SOURCE_DIR}"
    OUTPUT_VARIABLE out
    ERROR_VARIABLE out
    RESULT_VARIABLE retc
    )

if(retc)
    message(WARNING "Failed to install bikeshed in virtualenv [${retc}]:\n${out}")
    _fail_find("Could not install bikeshed in virtualenv")
endif()


_bikeshed_found()