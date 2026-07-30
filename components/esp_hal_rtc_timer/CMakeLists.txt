idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

# TODO(IDF-15992): potentially remove __ULP_BUILDV2 handling from esp_hal_rtc_timer
if(__ULP_BUILDV2)
    # LP-core timer shared code includes hal/rtc_timer_ll.h from
    # ulp/lp_core/shared/ulp_lp_core_lp_timer_shared.c.
    idf_component_register(INCLUDE_DIRS ${public_include}
                           REQUIRES soc hal esp_rom)
    return()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal esp_rom)
