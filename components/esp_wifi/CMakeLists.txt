idf_build_get_property(idf_target IDF_TARGET)

if(CONFIG_ESP32_NO_BLOBS OR CONFIG_ESP32S2_NO_BLOBS)
    set(link_binary_libs 0)
    set(ldfragments)
else()
    set(link_binary_libs 1)
    set(ldfragments "linker.lf")
endif()

idf_component_register(SRCS "src/coexist.c"
                            "src/lib_printf.c"
                            "src/mesh_event.c"
                            "src/phy_init.c"
                            "src/smartconfig.c"
                            "src/smartconfig_ack.c"
                            "src/wifi_init.c"
                            "src/wifi_default.c"
                            "src/wifi_netif.c"
                    INCLUDE_DIRS "include" "${idf_target}/include"
                    PRIV_REQUIRES wpa_supplicant nvs_flash esp_netif
                    LDFRAGMENTS "${ldfragments}")

idf_build_get_property(build_dir BUILD_DIR)
target_link_libraries(${COMPONENT_LIB} PUBLIC "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}")

if(link_binary_libs)
    if(CONFIG_IDF_TARGET_ESP32)
        set(phy phy)
    elseif(CONFIG_IDF_TARGET_ESP32S2BETA)
        # TODO: remove once final S2 chip is supported
        if(CONFIG_ESP32S2_BETA_VERSION_A)
            set(phy phyA)
        elseif(CONFIG_ESP32S2_BETA_VERSION_B)
            set(phy phyB)
        elseif(CONFIG_ESP32S2_BETA_VERSION_MARLIN3)
            set(phy phy_marlin3)
        endif()
    endif()
    set(blobs coexist core espnow mesh net80211 pp rtc smartconfig ${phy})

    foreach(blob ${blobs})
        add_library(${blob} STATIC IMPORTED)
        set_property(TARGET ${blob} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/lib${blob}.a)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${blob})

        foreach(_blob ${blobs})
            if(NOT _blob STREQUAL ${blob})
                set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${_blob})
            endif()
        endforeach()

        set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${COMPONENT_LIB})
    endforeach()
endif()

if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
    idf_component_get_property(esp_common_dir esp_common COMPONENT_DIR)
    partition_table_get_partition_info(phy_partition_offset "--partition-type data --partition-subtype phy" "offset")
    set(phy_init_data_bin "${build_dir}/phy_init_data.bin")

    # To get the phy_init_data.bin file, compile phy_init_data.h as a C file and then objcopy
    # the object file to a raw binary
    idf_build_get_property(config_dir CONFIG_DIR)
    add_custom_command(
        OUTPUT ${phy_init_data_bin}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${idf_target}/include/phy_init_data.h
        COMMAND ${CMAKE_C_COMPILER} -x c -c
        -I ${esp_common_dir}/include -I ${CMAKE_CURRENT_LIST_DIR}/include -I ${config_dir}
        -o phy_init_data.obj
        ${CMAKE_CURRENT_LIST_DIR}/${idf_target}/include/phy_init_data.h
        COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${phy_init_data_bin}
        )
    add_custom_target(phy_init_data ALL DEPENDS ${phy_init_data_bin})
    add_dependencies(flash phy_init_data)

    # ToDo: remove once MP chip is supported
    if(CONFIG_IDF_TARGET_ESP32)
        esptool_py_flash_project_args(phy ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
    elseif(CONFIG_IDF_TARGET_ESP32S2BETA)
        if(CONFIG_ESP32S2BETA_VERSION_A)
        esptool_py_flash_project_args(phyA ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
        elseif(CONFIG_ESP32S2BETA_VERSION_B)
        esptool_py_flash_project_args(phyB ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
        elseif(CONFIG_ESP32S2BETA_VERSION_MARLIN3)
        esptool_py_flash_project_args(phy_marlin3 ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
        endif()
    endif()
endif()
