idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
set(ldfragments)

if(CONFIG_ESP_RISCV_TRACE_ENABLE)
    list(APPEND srcs "src/esp_riscv_trace.c" "src/esp_riscv_trace_snapshot.c")
    if(CONFIG_ESP_COREDUMP_ENABLE)
        list(APPEND srcs "src/esp_riscv_trace_coredump.c")
        list(APPEND ldfragments "linker.lf")
    endif()
endif()

set(priv_requires)
if(NOT ${target} STREQUAL "linux")
    set(priv_requires esp_mm hal esp_app_format esp_hw_support esp_hal_debug_assist)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_INCLUDE_DIRS "src" "private_include"
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "${ldfragments}"
                       )

if(CONFIG_ESP_RISCV_TRACE_ENABLE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_panic_handler_inst_trace_stop")
    if(CONFIG_ESP_COREDUMP_ENABLE)
        idf_component_optional_requires(PRIVATE espcoredump)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_riscv_trace_coredump_include_func")
    endif()
endif()
