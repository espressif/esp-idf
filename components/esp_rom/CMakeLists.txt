if(BOOTLOADER_BUILD)
    # For bootloader, all we need is headers
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS)
    register_component()
    set(scripts
        "esp32/ld/esp32.rom.ld"
        "esp32/ld/esp32.rom.newlib-funcs.ld"
        "esp32/ld/esp32.rom.libgcc.ld"
        )
    target_linker_script(${COMPONENT_LIB} "${scripts}")
else()
    # Regular app build
    set(COMPONENT_SRCS "esp_rom.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    register_component()

    set(scripts
        "esp32/ld/esp32.rom.ld"
        "esp32/ld/esp32.rom.libgcc.ld"
        "esp32/ld/esp32.rom.syscalls.ld"
        "esp32/ld/esp32.rom.newlib-data.ld"
        )
    target_linker_script(${COMPONENT_LIB} "${scripts}")

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        # Note: Adding as a PUBLIC compile option here causes this option to propagate to all components that depend on esp32.
        #
        # To handle some corner cases, the same flag is set in project_include.cmake
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    else()
        target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-funcs.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-nano.ld")
    endif()

    if(NOT GCC_NOT_5_2_0)
        target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-locale.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.spiflash.ld")
    endif()
endif()
