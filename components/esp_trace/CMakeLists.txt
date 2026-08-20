idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return()
endif()

set(srcs)
if(CONFIG_ESP_TRACE_ENABLE)
    list(APPEND srcs
        "src/core/esp_trace_core.c"
        "src/core/esp_trace_registry.c"
        "src/ports/port_utils.c"
    )

    if(CONFIG_ESP_TRACE_TRANSPORT_APPTRACE)
        list(APPEND srcs "adapters/transport/adapter_transport_apptrace.c")
    endif()

    if(CONFIG_ESP_TRACE_TRANSPORT_USB_SERIAL_JTAG)
        list(APPEND srcs "adapters/transport/adapter_transport_usb_serial_jtag.c")
    endif()

    if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
        list(APPEND srcs "src/function_trace.c")
    endif()
endif()

set(includes
    "include"
)

set(priv_requires
    "esp_driver_gptimer"
    "esp_hal_usb"
    "esp_driver_usb_serial_jtag"
    "esp_hal_systimer"
    "esp_timer"
    "esp_system"
)
set(priv_includes "private_include")
set(requires "app_trace")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       PRIV_REQUIRES ${priv_requires}
                       REQUIRES ${requires}
                       WHOLE_ARCHIVE TRUE
                       LDFRAGMENTS linker.lf)

if(CONFIG_ESP_TRACE_OPTIMIZE_SPEED)
    target_compile_options(${COMPONENT_LIB} PRIVATE -O2)
endif()
