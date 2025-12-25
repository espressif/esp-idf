idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

# New chips with PMU peripheral
if(CONFIG_SOC_PMU_SUPPORTED)
    list(APPEND srcs "${target}/pmu_hal.c")
endif()

# Legacy chips with RTC_CNTL peripheral
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/rtc_cntl_hal.c")
    list(APPEND srcs "${target}/rtc_cntl_hal.c")
endif()

# Chips with PAU (Power Assist Unit) peripheral
if(CONFIG_SOC_PAU_SUPPORTED)
    list(APPEND srcs "${target}/pau_hal.c")
endif()

if(CONFIG_SOC_BOD_SUPPORTED)
    list(APPEND srcs "brownout_hal.c")
endif()

if(CONFIG_SOC_VBAT_SUPPORTED)
    list(APPEND srcs "vbat_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal esp_rom
                       LDFRAGMENTS linker.lf)
