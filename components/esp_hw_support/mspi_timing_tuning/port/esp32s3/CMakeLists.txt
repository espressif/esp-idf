target_include_directories(${COMPONENT_LIB} PUBLIC . include)

set(srcs)

if(NOT BOOTLOADER_BUILD)
    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        list(APPEND srcs "mspi_timing_config.c" "mspi_timing_by_mspi_delay.c")
    endif()
endif()

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})
