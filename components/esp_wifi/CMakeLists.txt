set(COMPONENT_SRCS
    "src/coexist.c"
    "src/fast_crypto_ops.c"
    "src/lib_printf.c"
    "src/mesh_event.c"
    "src/phy_init.c"
    "src/restore.c"
    "src/wifi_init.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS)
set(COMPONENT_REQUIRES wpa_supplicant smartconfig_ack)
set(COMPONENT_PRIV_REQUIRES "nvs_flash")

set(link_binary_libs 1)
if (CONFIG_ESP32_NO_BLOBS OR CONFIG_ESP32S2_NO_BLOBS)
    set(link_binary_libs 0)
endif()

if (CONFIG_IDF_TARGET_ESP32S2BETA)
    message(WARNING "No Wi-Fi libraries yet for esp32s2beta")
    set(link_binary_libs 0)
endif()

if(link_binary_libs)
    set(COMPONENT_ADD_LDFRAGMENTS "linker.lf")
endif()

register_component()
target_link_libraries(${COMPONENT_LIB} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib_${IDF_TARGET}")

if(link_binary_libs)
    set(blobs coexist core espnow mesh net80211 phy pp rtc smartconfig wpa2 wpa wps)
    foreach(blob ${blobs})
        add_library(${blob} STATIC IMPORTED)
        # set_property(TARGET ${blob} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib_${IDF_TARGET}/lib${blob}.a)
        # ToDo: add lib_esp32s2beta in esp_wifi component
        set_property(TARGET ${blob} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib_esp32/lib${blob}.a)
        target_link_libraries(${COMPONENT_LIB} ${blob})

        foreach(_blob ${blobs})
            if(NOT _blob STREQUAL ${blob})
                set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${_blob})
            endif()
        endforeach()

        set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${COMPONENT_LIB})
    endforeach()
endif()

if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
    idf_build_get_property(build_dir BUILD_DIR)
    partition_table_get_partition_info(phy_partition_offset "--partition-type data --partition-subtype phy" "offset")
    set(phy_init_data_bin "${build_dir}/phy_init_data.bin")

    # To get the phy_init_data.bin file, compile phy_init_data.h as a C file and then objcopy
    # the object file to a raw binary
    add_custom_command(
        OUTPUT ${phy_init_data_bin}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
        COMMAND ${CMAKE_C_COMPILER} -x c -c
        -I ${CMAKE_CURRENT_LIST_DIR} -I ${CMAKE_CURRENT_LIST_DIR}/include -I ${build_dir}
        -o phy_init_data.obj
        ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
        COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${phy_init_data_bin}
        )
    add_custom_target(phy_init_data ALL DEPENDS ${phy_init_data_bin})
    add_dependencies(app phy_init_data)

    esptool_py_flash_project_args(phy ${phy_partition_offset} ${phy_init_data_bin} FLASH_IN_PROJECT)
endif()
