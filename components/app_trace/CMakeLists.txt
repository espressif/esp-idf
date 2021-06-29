set(srcs 
    "app_trace.c"
    "app_trace_util.c"
    "host_file_io.c"
    "gcov/gcov_rtio.c")

set(include_dirs "include")

if(CONFIG_SYSVIEW_ENABLE)
    list(APPEND include_dirs
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/OS)

    list(APPEND srcs 
        "sys_view/SEGGER/SEGGER_SYSVIEW.c"
        "sys_view/Sample/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c"
        "sys_view/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.c"
        "sys_view/esp32/SEGGER_RTT_esp32.c"
        "sys_view/ext/heap_trace_module.c"
        "sys_view/ext/logging.c")
endif()

if(CONFIG_HEAP_TRACING_TOHOST)
    list(APPEND srcs "heap_trace_tohost.c")
    set_source_files_properties(heap_trace_tohost.c
        PROPERTIES COMPILE_FLAGS
        -Wno-frame-address)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES soc esp_ipc
                       LDFRAGMENTS linker.lf)


if(CONFIG_APPTRACE_GCOV_ENABLE)
    # disable --coverage for this component, as it is used as transport
    # for gcov
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-profile-arcs" "-fno-test-coverage")

    # Force app_trace to also appear later than gcov in link line
    idf_component_get_property(app_trace app_trace COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} INTERFACE
        "-Wl,--undefined=gcov_rtio_atexit" $<TARGET_FILE:${app_trace}> gcov $<TARGET_FILE:${app_trace}> c)
endif()
