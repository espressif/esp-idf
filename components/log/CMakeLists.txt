list(APPEND srcs "log.c"
                 "log_buffers.c")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES soc)

idf_build_get_property(build_components BUILD_COMPONENTS)
# Ideally, FreeRTOS shouldn't be included into bootloader build, so the 2nd check should be unnecessary
if(freertos IN_LIST BUILD_COMPONENTS AND NOT BOOTLOADER_BUILD)
    target_sources(${COMPONENT_TARGET} PRIVATE log_freertos.c)
else()
    target_sources(${COMPONENT_TARGET} PRIVATE log_noos.c)
endif()
