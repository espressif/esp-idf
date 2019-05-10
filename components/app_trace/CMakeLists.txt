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
                   "sys_view/esp32/SEGGER_RTT_esp32.c"
                   "sys_view/ext/heap_trace_module.c"
                   "sys_view/ext/logging.c")
endif()

if(CONFIG_HEAP_TRACING_TOHOST)
    list(APPEND COMPONENT_SRCS "heap_trace_tohost.c")
endif()

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES heap soc)
set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()

# disable --coverage for this component, as it is used as transport
# for gcov
target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-profile-arcs" "-fno-test-coverage")
target_link_libraries(${COMPONENT_LIB} gcov ${LIBC} ${LIBM})
