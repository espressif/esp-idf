set(COMPONENT_SRCDIRS "." "gcov")
set(COMPONENT_ADD_INCLUDEDIRS "include")

if(CONFIG_SYSVIEW_ENABLE)
    list(APPEND COMPONENT_ADD_INCLUDEDIRS
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/OS)

    list(APPEND COMPONENT_SRCDIRS
        "sys_view/SEGGER"
        "sys_view/Sample/OS"
        "sys_view/Sample/Config"
        "sys_view/esp32")
endif()

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES xtensa-debug-module)

register_component()

# disable --coverage for this component, as it is used as transport
# for gcov
component_compile_options("-fno-profile-arcs" "-fno-test-coverage")
