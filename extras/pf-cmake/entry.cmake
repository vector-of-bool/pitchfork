function(_pf_download url dest)
    set(tmp "${dest}.tmp")
    file(
        DOWNLOAD "${url}"
        "${tmp}"
        STATUS st
        )
    list(GET st 0 rc)
    list(GET st 1 msg)
    if(rc)
        message(FATAL_ERROR "Error while downloading file [${rc}]: ${msg}")
    endif()
    file(RENAME "${tmp}" "${dest}")
endfunction()

foreach(fname IN ITEMS auto.cmake)
    get_filename_component(_pf_dest "${_PF_DIR}/${fname}" ABSOLUTE)
    _pf_download("${PF_URL}/${fname}" "${_pf_dest}")
    include("${_pf_dest}")
endforeach()
