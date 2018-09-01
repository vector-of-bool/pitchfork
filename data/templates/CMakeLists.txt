file(
    GLOB_RECURSE template_files
    CONFIGURE_DEPENDS
    base/*
    cmake/*
    )
cmrc_add_resource_library(
    pf_templates
    ALIAS pf::templates
    ${template_files}
    )
