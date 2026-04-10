idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND public_include "${target}/include")
endif()

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "${target}/twai_periph.c")

    if(CONFIG_SOC_TWAI_FD_SUPPORTED)
        list(APPEND srcs "twai_hal_v2.c")
    else()
        list(APPEND srcs "twai_hal_v1.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
