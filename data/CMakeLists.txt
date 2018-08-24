if(TARGET Bikeshed::Bikeshed)
    get_filename_component(bs_input "spec.bs" ABSOLUTE)
    get_filename_component(bs_html "${PROJECT_BINARY_DIR}/spec.html" ABSOLUTE)

    add_custom_command(
        OUTPUT "${bs_html}"
        SOURCES "${bs_input}"
        COMMAND Bikeshed::Bikeshed spec "${bs_input}" "${bs_html}"
        COMMENT "Rendering specification to ${bs_html}"
        )
    add_custom_target(pf-spec ALL DEPENDS "${bs_html}")
    add_custom_target(pf-spec-watch
        COMMAND Bikeshed::Bikeshed watch "${bs_input}" "${bs_html}"
        COMMENT "Watching ${bs_input} for changes"
        USES_TERMINAL
        )
endif()