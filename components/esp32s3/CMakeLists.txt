idf_build_get_property(target IDF_TARGET)
idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)

if(NOT "${target}" STREQUAL "esp32s3")
    return()
endif()

if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32s3 is headers
    idf_component_register(INCLUDE_DIRS include REQUIRES xtensa)
    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32s3.peripherals.ld")
else()
    # Regular app build

    set(srcs "cache_err_int.c"
             "clk.c"
             "crosscore_int.c"
             "dport_access.c"
             "esp_crypto_lock.c"
             "hw_random.c"

             "memprot.c"
             "spiram.c"
             "spiram_psram.c"
             "system_api_esp32s3.c")
    set(include_dirs "include")

    set(requires driver efuse soc xtensa) #unfortunately rom/uart uses SOC registers directly

    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    # esp_timer is added here because cpu_start.c uses esp_timer
    set(priv_requires app_trace app_update bootloader_support log mbedtls nvs_flash pthread
        spi_flash vfs espcoredump esp_common perfmon esp_timer esp_ipc)
    set(fragments linker.lf ld/esp32s3_fragments.lf)

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                        LDFRAGMENTS "${fragments}"
                        REQUIRES "${requires}"
                        PRIV_REQUIRES "${priv_requires}"
                        REQUIRED_IDF_TARGETS esp32s3)

    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/esp32s3_out.ld")

    # Process the template file through the linker script generation mechanism, and use the output for linking the
    # final binary
    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/ld/esp32s3.project.ld.in"
                                          PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/esp32s3.project.ld")

    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32s3.peripherals.ld")
    target_link_libraries(${COMPONENT_LIB} PUBLIC gcc)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u call_user_start_cpu0")

    idf_build_get_property(config_dir CONFIG_DIR)
    # Preprocess esp32s3.ld linker script to include configuration, becomes esp32s3_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32s3_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32s3_out.ld -I ${config_dir} ${LD_DIR}/esp32s3.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32s3.ld ${sdkconfig_header}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32s3_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32s3_out.ld)
    add_dependencies(${COMPONENT_LIB} esp32s3_linker_script)

    # disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        cpu_start.c
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
endif()
