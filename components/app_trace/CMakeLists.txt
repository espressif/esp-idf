idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_ESP_TRACE_TRANSPORT_APPTRACE)
    set(srcs
        "app_trace.c"
        "app_trace_util.c"
        "host_file_io.c"
        )

    if(NOT CONFIG_APPTRACE_DEST_UART)  # JTAG or None
        if(CONFIG_IDF_TARGET_ARCH_XTENSA)
            list(APPEND srcs "port/xtensa/port_jtag.c")
        elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
            list(APPEND srcs "port/riscv/port_jtag.c")
        endif()
        list(APPEND srcs "app_trace_membufs_proto.c")
    endif()

    if(NOT CONFIG_APPTRACE_DEST_JTAG)  # UART or None
        list(APPEND srcs "port/port_uart.c")
    endif()
endif()

if(CONFIG_ESP_DEBUG_STUBS_ENABLE)
    list(APPEND srcs "debug_stubs.c")
endif()

set(include_dirs "include")

set(priv_include_dirs "private_include" "port/include")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       PRIV_REQUIRES esp_driver_gptimer
                       REQUIRES esp_timer esp_driver_uart
                       LDFRAGMENTS linker.lf)
