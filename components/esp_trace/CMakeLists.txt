idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return()
endif()

if(CONFIG_ESP_TRACE_ENABLE)
    set(srcs
        "src/core/esp_trace_core.c"
        "src/core/esp_trace_registry.c"
        "src/ports/port_utils.c"
    )

    if(CONFIG_ESP_TRACE_TRANSPORT_APPTRACE)
        list(APPEND srcs "adapters/transport/adapter_transport_apptrace.c")
    endif()

    if(CONFIG_ESP_TRACE_LIB_SEGGER_SYSVIEW)
        list(APPEND srcs "adapters/encoder/adapter_encoder_sysview.c")
    endif()

endif()

set(includes
    "include"
)

set(priv_requires "esp_driver_gptimer")
set(priv_includes "")
set(requires "app_trace")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       PRIV_REQUIRES ${priv_requires}
                       REQUIRES ${requires}
                       WHOLE_ARCHIVE TRUE
                       LDFRAGMENTS linker.lf)
