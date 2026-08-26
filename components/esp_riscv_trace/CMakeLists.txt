idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")

if(CONFIG_ESP_RISCV_TRACE_ENABLE)
    list(APPEND srcs "src/esp_riscv_trace.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
    set(requires "")
else()
    set(priv_requires esp_mm)
    set(requires esp_hal_debug_assist)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_INCLUDE_DIRS "src"
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                       )
