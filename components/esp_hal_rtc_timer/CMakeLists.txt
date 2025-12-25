idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal esp_rom)
