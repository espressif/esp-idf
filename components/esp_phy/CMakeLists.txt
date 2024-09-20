idf_build_get_property(idf_target IDF_TARGET)

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
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${idf_target}/phy_multiple_init_data.bin DESTINATION "${build_dir}")
    endif()
endif()

if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED)
    set(embed_files "${build_dir}/phy_multiple_init_data.bin")
endif()
endif()

# [refactor-todo]: requires "driver" component for periph_ctrl header file
idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "${idf_target}/include"
                    PRIV_REQUIRES nvs_flash driver efuse esp_timer esp_wifi
                    LDFRAGMENTS "${ldfragments}"
                    EMBED_FILES  ${embed_files}
                    )

set(target_name "${idf_target}")
target_link_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}")

# Override functions in PHY lib with the functions in 'phy_override.c'
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u include_esp_phy_override")

if(link_binary_libs)
    target_link_libraries(${COMPONENT_LIB} PUBLIC phy)

    idf_component_get_property(esp_phy_lib esp_phy COMPONENT_LIB)

    if(CONFIG_ESP_PHY_ENABLE_CERT_TEST)
        if(CONFIG_IDF_TARGET_ESP32)
            target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libbttestmode.a
            librftest.a $<TARGET_FILE:${esp_phy_lib}>)
        elseif(CONFIG_IDF_TARGET_ESP32S2)
            target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> librftest.a
            librfate.a $<TARGET_FILE:${esp_phy_lib}>)
        else()
            target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libbttestmode.a
            librfate.a librftest.a $<TARGET_FILE:${esp_phy_lib}>)
        endif()
    endif()

    if(CONFIG_IDF_TARGET_ESP32)
        target_link_libraries(${COMPONENT_LIB} PUBLIC rtc)
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libphy.a librtc.a
        $<TARGET_FILE:${esp_phy_lib}>)
    elseif(CONFIG_SOC_BT_SUPPORTED OR CONFIG_SOC_IEEE802154_SUPPORTED)
        target_link_libraries(${COMPONENT_LIB} PUBLIC btbb)
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libphy.a libbtbb.a
        $<TARGET_FILE:${esp_phy_lib}>)
    elseif(CONFIG_SOC_WIFI_SUPPORTED)
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libphy.a
        $<TARGET_FILE:${esp_phy_lib}>)
    endif()
endif()

if(CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION)
    idf_component_get_property(esp_common_dir esp_common COMPONENT_DIR)
    partition_table_get_partition_info(phy_partition_offset "--partition-type data --partition-subtype phy" "offset")

    if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN)
        set(phy_init_data_bin "${build_dir}/phy_multiple_init_data.bin")
        if(CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED)
            set(COMPONENT_EMBED_FILES "${build_dir}/phy_multiple_init_data.bin")
        endif()
    else()
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

        idf_component_get_property(main_args esptool_py FLASH_ARGS)
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
    endif()

    set(phy_name "phy")

    esptool_py_flash_target(${phy_name}-flash "${main_args}" "${sub_args}")
    esptool_py_flash_target_image(${phy_name}-flash ${phy_name} "${phy_partition_offset}" "${phy_init_data_bin}")
    esptool_py_flash_target_image(flash ${phy_name} "${phy_partition_offset}" "${phy_init_data_bin}")
endif()
