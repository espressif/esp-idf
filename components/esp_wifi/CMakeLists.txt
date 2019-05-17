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
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES "wpa_supplicant" "nvs_flash")

if(NOT CONFIG_NO_BLOBS)
    set(COMPONENT_ADD_LDFRAGMENTS "linker.lf")
endif()

register_component()
target_link_libraries(${COMPONENT_LIB} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib_${IDF_TARGET}")

if(NOT CONFIG_NO_BLOBS)
    target_link_libraries(${COMPONENT_LIB} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib_${IDF_TARGET}")
    target_link_libraries(${COMPONENT_LIB} coexist core espnow mesh net80211 phy pp rtc smartconfig wpa2 wpa wps)
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