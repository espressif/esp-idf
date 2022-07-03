set(srcs
    "app_trace.c"
    "app_trace_util.c"
    "host_file_io.c"
    "gcov/gcov_rtio.c")

set(include_dirs "include")

set(priv_include_dirs "private_include" "port/include")

if(CONFIG_APPTRACE_MEMBUFS_APPTRACE_PROTO_ENABLE)
    list(APPEND srcs
        "app_trace_membufs_proto.c")

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        list(APPEND srcs
            "port/xtensa/port.c")
    endif()
    if(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND srcs
            "port/riscv/port.c")
    endif()
endif()
list(APPEND srcs
    "port/port_uart.c")

if(CONFIG_APPTRACE_SV_ENABLE)
    list(APPEND include_dirs
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/OS)

    list(APPEND srcs
        "sys_view/SEGGER/SEGGER_SYSVIEW.c"
        "sys_view/Sample/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c"
        "sys_view/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.c"
        "sys_view/esp/SEGGER_RTT_esp.c"
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
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       # Requires "driver" for GPTimer in "SEGGER_SYSVIEW_Config_FreeRTOS.c"
                       PRIV_REQUIRES soc driver
                       REQUIRES esp_timer
                       LDFRAGMENTS linker.lf)

# disable --coverage for this component, as it is used as transport
# for gcov
target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-profile-arcs" "-fno-test-coverage")

# Force app_trace to also appear later than gcov in link line
idf_component_get_property(app_trace app_trace COMPONENT_LIB)
target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${app_trace}> gcov $<TARGET_FILE:${app_trace}> c)

# This function adds a dependency on the given component if the component is included into the build.
function(maybe_add_component component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if(${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${lib_name})
    endif()
endfunction()

if(CONFIG_APPTRACE_DEST_UART0 OR CONFIG_APPTRACE_DEST_UART1 OR CONFIG_APPTRACE_DEST_UART2)
    maybe_add_component(driver)
endif()
