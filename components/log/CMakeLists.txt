idf_build_get_property(target IDF_TARGET)
set(srcs "log.c" "log_buffers.c")
set(priv_requires "")
if(${target} STREQUAL "linux")
    list(APPEND srcs "log_linux.c")
else()
    list(APPEND priv_requires soc hal esp_hw_support)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES ${priv_requires})

if(NOT ${target} STREQUAL "linux")
    # Ideally, FreeRTOS shouldn't be included into bootloader build, so the 2nd check should be unnecessary
    if(freertos IN_LIST BUILD_COMPONENTS AND NOT BOOTLOADER_BUILD)
        target_sources(${COMPONENT_TARGET} PRIVATE log_freertos.c)
    else()
        target_sources(${COMPONENT_TARGET} PRIVATE log_noos.c)
    endif()
endif()
