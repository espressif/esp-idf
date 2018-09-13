set(COMPONENT_SRCS "app_trace.c"
                   "app_trace_util.c"
                   "host_file_io.c"
                   "gcov/gcov_rtio.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

if(CONFIG_SYSVIEW_ENABLE)
    list(APPEND COMPONENT_ADD_INCLUDEDIRS
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/OS)

    list(APPEND COMPONENT_SRCS "sys_view/SEGGER/SEGGER_SYSVIEW.c"
                   "sys_view/Sample/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c"
                   "sys_view/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.c"
                   "sys_view/esp32/SEGGER_RTT_esp32.c")
endif()

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES xtensa-debug-module)

register_component()

# disable --coverage for this component, as it is used as transport
# for gcov
component_compile_options("-fno-profile-arcs" "-fno-test-coverage")
