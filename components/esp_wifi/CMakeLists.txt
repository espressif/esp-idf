idf_build_get_property(idf_target IDF_TARGET)

if(NOT CONFIG_ESP32_NO_BLOBS)
    set(ldfragments "linker.lf")
endif()

idf_component_register(SRCS "src/coexist.c"
                            "src/lib_printf.c"
                            "src/mesh_event.c"
                            "src/phy_init.c"
                            "src/smartconfig.c"
                            "src/smartconfig_ack.c"
                            "src/wifi_init.c"
                    INCLUDE_DIRS "include" "${idf_target}/include"
                    PRIV_REQUIRES wpa_supplicant nvs_flash
                    LDFRAGMENTS "${ldfragments}")

idf_build_get_property(build_dir BUILD_DIR)
target_link_libraries(${COMPONENT_LIB} PUBLIC "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib_${idf_target}")

if(NOT CONFIG_ESP32_NO_BLOBS)
    set(blobs coexist core espnow mesh net80211 phy pp rtc smartconfig)
    foreach(blob ${blobs})
        add_library(${blob} STATIC IMPORTED)
        set_property(TARGET ${blob} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib_${idf_target}/lib${blob}.a)
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

    esptool_py_flash_project_args(phy ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
endif()
