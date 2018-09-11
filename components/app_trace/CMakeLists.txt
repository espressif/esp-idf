set(COMPONENT_SRCS "app_trace.c"
                   "app_trace_util.c"
                   "gcov/gcov_rtio.c"
                   "host_file_io.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

if(CONFIG_SYSVIEW_ENABLE)
    list(APPEND COMPONENT_ADD_INCLUDEDIRS
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/OS)

    set(COMPONENT_SRCS "SEGGER/SEGGER_SYSVIEW.c"
                    "esp32/SEGGER_RTT_esp32.c"
                    "Sample/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c"
                    "Sample/OS/SEGGER_SYSVIEW_FreeRTOS.c")
endif()

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES xtensa-debug-module)

register_component()

# disable --coverage for this component, as it is used as transport
# for gcov
component_compile_options("-fno-profile-arcs" "-fno-test-coverage")
