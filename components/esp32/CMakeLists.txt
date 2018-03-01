if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32 is headers
    add_library(esp32 INTERFACE)
    target_include_directories(esp32 INTERFACE include)

    # as cmake won't attach linker args to a header-only library, attach
    # linker args directly to the bootloader.elf
    set(BOOTLOADER_LINKER_ARGS
        "-L ${CMAKE_CURRENT_SOURCE_DIR}/ld"
        "-T esp32.rom.ld"
        "-T esp32.rom.spiram_incompatible_fns.ld"
        "-T esp32.peripherals.ld"
        PARENT_SCOPE)

else()
    # Regular app build

    set(COMPONENT_SRCDIRS ". hwcrypto")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    register_component()

    target_link_libraries(esp32 "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
    if(NOT CONFIG_NO_BLOBS)
        target_link_libraries(esp32 coexist core espnow net80211 phy pp rtc smartconfig wpa2 wpa wps)
    endif()
    target_linker_script(esp32 "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    target_linker_script(esp32 "ld/esp32.common.ld")
    target_linker_script(esp32 "ld/esp32.rom.ld")
    target_linker_script(esp32 "ld/esp32.peripherals.ld")

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        add_compile_options(-mfix-esp32-psram-cache-issue)
    else()
        target_linker_script(esp32 "ld/esp32.rom.spiram_incompatible_fns.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(esp32 "ld/esp32.rom.nanofmt.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(esp32 "ld/esp32.rom.spiflash.ld")
    endif()

    target_link_libraries(esp32 "${CMAKE_CURRENT_SOURCE_DIR}/libhal.a")
    target_link_libraries(esp32 gcc)
    target_link_libraries(esp32 "-u call_user_start_cpu0")

    #ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
    #linker will ignore panic_highint_hdl.S as it has no other files depending on any
    #symbols in it.
    target_link_libraries(esp32 "-u ld_include_panic_highint_hdl")

    # Preprocess esp32.ld linker script to include configuration, becomes esp32_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32_out.ld -I ${CMAKE_BINARY_DIR} ${LD_DIR}/esp32.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32.ld
        COMMENT "Generating linker script..."
        VERBATIM)
    add_custom_target(esp32_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld)
    add_dependencies(esp32 esp32_linker_script)

    if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
        set(PHY_INIT_DATA_BIN phy_init_data.bin)

        # To get the phy_init_data.bin file, compile phy_init_data.h as a C file and then objcopy
        # the object file to a raw binary
        add_custom_command(
            OUTPUT ${PHY_INIT_DATA_BIN}
            DEPENDS ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_C_COMPILER} -x c -c
            -I ${CMAKE_CURRENT_LIST_DIR} -I ${CMAKE_CURRENT_LIST_DIR}/include -I ${CMAKE_BINARY_DIR}
            -o phy_init_data.obj
            ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${PHY_INIT_DATA_BIN}
            )
        add_custom_target(phy_init_data ALL DEPENDS ${PHY_INIT_DATA_BIN})
        add_dependencies(flash phy_init_data)

    endif()

endif()
