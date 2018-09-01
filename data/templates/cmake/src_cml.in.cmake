add_library(
    {{project_name}}
    {{ns_path}}/{{first_stem}}.hpp
    {{ns_path}}/{{first_stem}}.cpp
    )
add_library({{alias_target}} ALIAS {{project_name}})
target_include_directories({{project_name}}
    {{ #separate_headers %}}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    {{% /separate_headers %}}
    {{% ^separate_headers %}}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    {{% /separate_headers }}
    )
