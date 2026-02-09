idf_build_get_property(idf_target IDF_TARGET)

if(${idf_target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(IDF_TARGET STREQUAL "esp32p4")
    # TODO: IDF-7460
    idf_component_register()
    return()
endif()

if(CONFIG_ESP_PHY_ENABLED)

    set(srcs "src/phy_override.c" "src/lib_printf.c" "src/phy_common.c")

    if(CONFIG_APP_NO_BLOBS)
        set(link_binary_libs 0)
        set(ldfragments)
    else()
        set(link_binary_libs 1)
        set(ldfragments "linker.lf")
    endif()

    if(CONFIG_SOC_IEEE802154_BLE_ONLY)
        list(APPEND srcs "src/phy_init_esp32hxx.c")
    else()
        list(APPEND srcs "src/phy_init.c")
        list(APPEND srcs "${idf_target}/phy_init_data.c")
    endif()

    if(CONFIG_SOC_BT_SUPPORTED OR CONFIG_SOC_IEEE802154_SUPPORTED OR CONFIG_SOC_IEEE802154_BLE_ONLY)
        list(APPEND srcs "src/btbb_init.c")
    endif()

    if(CONFIG_ESP_PHY_ENABLE_CERT_TEST)
        list(APPEND srcs "src/phy_callback.c")
    endif()

    idf_build_get_property(build_dir BUILD_DIR)

    if(CONFIG_SOC_WIFI_SUPPORTED)
        if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN)
            if(NOT EXISTS "${build_dir}/phy_multiple_init_data.bin")
                file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${idf_target}/phy_multiple_init_data.bin
                DESTINATION "${build_dir}")
            endif()
        endif()

        if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED)
            set(embed_files "${build_dir}/phy_multiple_init_data.bin")
        endif()
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "${idf_target}/include"
                    # 'esp_hal_ana_conv' is required by 'phy_common.c' for the temperature sensor hal
                    PRIV_REQUIRES nvs_flash esp_driver_gpio esp_hal_ana_conv efuse esp_timer esp_wifi esptool_py
                    LDFRAGMENTS "${ldfragments}"
                    EMBED_FILES  ${embed_files}
                    )

if(CONFIG_ESP_PHY_ENABLED)
    set(lib_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}")

    # Override functions in PHY lib with the functions in 'phy_override.c'
    target_link_options(${COMPONENT_LIB} INTERFACE "SHELL:-u include_esp_phy_override")

    if(link_binary_libs)

        # Decide which libraries to link based on the configuration and the target
        set(has_libbttestmode 0)
        set(has_librftest 0)
        set(has_librfate 0)
        set(has_libphy 0)
        set(has_libbtbb 0)
        set(has_librtc 0)

        if(CONFIG_ESP_PHY_ENABLE_CERT_TEST)
            if(CONFIG_IDF_TARGET_ESP32)
                set(has_libbttestmode 1)
                set(has_librftest 1)
            elseif(CONFIG_IDF_TARGET_ESP32S2)
                set(has_librftest 1)
                set(has_librfate 1)
            else()
                set(has_libbttestmode 1)
                set(has_librftest 1)
                set(has_librfate 1)
            endif()
        endif()

        if(CONFIG_IDF_TARGET_ESP32)
            set(has_librtc 1)
            set(has_libphy 1)
        elseif(CONFIG_SOC_BT_SUPPORTED OR CONFIG_SOC_IEEE802154_SUPPORTED)
            set(has_libbtbb 1)
            set(has_libphy 1)
        elseif(CONFIG_SOC_WIFI_SUPPORTED)
            set(has_libphy 1)
        endif()

        # Add the required library targets and add them to the list of libraries to link
        if(has_libbttestmode)
            add_prebuilt_library(esp_phy_libbttestmode "${lib_dir}/libbttestmode.a"
                                REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_libbttestmode)
        endif()

        if(has_librftest)
            add_prebuilt_library(esp_phy_librftest "${lib_dir}/librftest.a"
                                REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_librftest)
        endif()

        if(has_librfate)
            add_prebuilt_library(esp_phy_librfate "${lib_dir}/librfate.a"
                                REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_librfate)
        endif()

        if(has_libbtbb)
            add_prebuilt_library(esp_phy_libbtbb "${lib_dir}/libbtbb.a"
                                 REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_libbtbb)
        endif()

        if(has_librtc)
            add_prebuilt_library(esp_phy_librtc "${lib_dir}/librtc.a"
                                 REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_librtc)
        endif()

        if(has_libphy)
            add_prebuilt_library(esp_phy_libphy "${lib_dir}/libphy.a"
                                 REQUIRES ${COMPONENT_NAME})
            list(APPEND libs_to_link esp_phy_libphy)
        endif()

        # Finally, link the libraries to the component
        target_link_libraries(${COMPONENT_LIB} INTERFACE ${libs_to_link})
    endif()

   if(CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION)
        idf_component_get_property(esp_common_dir esp_common COMPONENT_DIR)
        idf_component_get_property(esp_rom_dir esp_rom COMPONENT_DIR)
        idf_component_get_property(esp_libc_dir esp_libc COMPONENT_DIR)
        idf_component_get_property(soc_dir soc COMPONENT_DIR)
        idf_component_get_property(log_dir log COMPONENT_DIR)
        partition_table_get_partition_info(phy_partition_offset
        "--partition-type data --partition-subtype phy" "offset")

        if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN)
            set(phy_init_data_bin "${build_dir}/phy_multiple_init_data.bin")
            if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED)
                set(COMPONENT_EMBED_FILES "${build_dir}/phy_multiple_init_data.bin")
            endif()
        else()
            set(phy_init_data_bin "${build_dir}/phy_init_data.bin")

            idf_build_get_property(config_dir CONFIG_DIR)
            add_custom_command(
                OUTPUT ${phy_init_data_bin}
                DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${idf_target}/phy_init_data.c
                COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_LIST_DIR}/${idf_target}/phy_init_data.c
                -I ${esp_common_dir}/include -I ${CMAKE_CURRENT_LIST_DIR}/include
                -I ${CMAKE_CURRENT_LIST_DIR}/${idf_target}/include -I ${esp_rom_dir}/include
                -I ${soc_dir}/${idf_target}/include -I ${esp_libc_dir}/platform_include
                -I ${log_dir}/include -I ${config_dir} -o phy_init_data.obj
                COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${phy_init_data_bin}
                )
            add_custom_target(phy_init_data ALL DEPENDS ${phy_init_data_bin})
            add_dependencies(flash phy_init_data)

            idf_component_get_property(main_args esptool_py FLASH_ARGS)
            idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        endif()

        set(phy_name "phy")

        esptool_py_flash_target(${phy_name}-flash "${main_args}" "${sub_args}")
        esptool_py_flash_target_image(${phy_name}-flash ${phy_name} "${phy_partition_offset}" "${phy_init_data_bin}")
        esptool_py_flash_target_image(flash ${phy_name} "${phy_partition_offset}" "${phy_init_data_bin}")
    endif()

endif()
