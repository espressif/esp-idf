idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_APPTRACE_ENABLE)
    set(srcs
        "app_trace.c"
        "app_trace_util.c"
        "host_file_io.c"
        )

    if(NOT CONFIG_APPTRACE_DEST_UART)  # JTAG or ALL
        if(CONFIG_IDF_TARGET_ARCH_XTENSA)
            list(APPEND srcs "port/xtensa/port_jtag.c")
        elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
            list(APPEND srcs "port/riscv/port_jtag.c")
        endif()
        list(APPEND srcs "app_trace_membufs_proto.c")
    endif()

    if(NOT CONFIG_APPTRACE_DEST_JTAG)  # UART or ALL
        list(APPEND srcs "port/port_uart.c")
    endif()
endif()

if(CONFIG_ESP_DEBUG_STUBS_ENABLE)
    list(APPEND srcs "debug_stubs.c")
endif()

set(include_dirs "include")

set(priv_include_dirs "private_include" "port/include")

if(CONFIG_APPTRACE_SV_ENABLE)
    list(APPEND include_dirs
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/FreeRTOSV10.4)

    list(APPEND srcs
        "sys_view/SEGGER/SEGGER_SYSVIEW.c"
        "sys_view/Sample/FreeRTOSV10.4/Config/esp/SEGGER_SYSVIEW_Config_FreeRTOS.c"
        "sys_view/Sample/FreeRTOSV10.4/SEGGER_SYSVIEW_FreeRTOS.c"
        "sys_view/esp/SEGGER_RTT_esp.c"
        "sys_view/ext/heap_trace_module.c"
        "sys_view/ext/logging.c")
endif()

if(CONFIG_HEAP_TRACING_TOHOST)
    list(APPEND srcs "heap_trace_tohost.c")
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        set_source_files_properties(heap_trace_tohost.c PROPERTIES COMPILE_FLAGS -Wno-frame-address)
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       PRIV_REQUIRES esp_driver_gptimer
                       REQUIRES esp_timer esp_driver_uart
                       LDFRAGMENTS linker.lf)
