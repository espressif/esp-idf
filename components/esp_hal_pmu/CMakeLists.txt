idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")
# Public PMU/RTC control HAL headers include hal/regi2c_ctrl.h on several
# targets, so CMake v2 builds need esp_hal_regi2c explicitly.
set(requires soc hal esp_rom esp_hal_regi2c)

# TODO(IDF-15949): potentially remove __ULP_BUILD handling from esp_hal_pmu
if(__ULP_BUILD)
    # LP-core runtime code includes hal/pmu_ll.h from
    # ulp/lp_core/lp_core/lp_core_utils.c. Some target brownout/VBAT headers
    # include hal/regi2c_ctrl.h.
    idf_component_register(INCLUDE_DIRS ${public_include}
                           REQUIRES ${requires})
    return()
endif()

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
                       REQUIRES ${requires}
                       LDFRAGMENTS linker.lf)
