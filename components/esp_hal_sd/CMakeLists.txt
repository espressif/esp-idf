idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND public_include "${target}/include")
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "sdmmc_hal.c")
    list(APPEND srcs "${target}/sdmmc_periph.c")
endif()

if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "sdio_slave_hal.c")
    list(APPEND srcs "${target}/sdio_slave_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
