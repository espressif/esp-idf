idf_build_get_property(target IDF_TARGET)
set(srcs "log.c")
set(priv_requires "")
set(requires "")
if(${target} STREQUAL "linux")
    # We leave log buffers out for now on Linux since it's rarely used. Excplicitely add esp_rom to Linux target
    # since we don't have the common components there yet.
    list(APPEND srcs "log_linux.c")
    list(APPEND requires esp_rom)
else()
    list(APPEND srcs "log_buffers.c")
    list(APPEND priv_requires soc)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    LDFRAGMENTS linker.lf
                    REQUIRES ${requires}
                    PRIV_REQUIRES ${priv_requires})

if(NOT ${target} STREQUAL "linux")
    # Ideally, FreeRTOS shouldn't be included into bootloader build, so the 2nd check should be unnecessary
    if(freertos IN_LIST BUILD_COMPONENTS AND NOT BOOTLOADER_BUILD)
        target_sources(${COMPONENT_TARGET} PRIVATE log_freertos.c)
    else()
        target_sources(${COMPONENT_TARGET} PRIVATE log_noos.c)
    endif()
endif()
