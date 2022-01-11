if(CONFIG_ESP32_WIFI_ENABLED)
    idf_build_get_property(idf_target IDF_TARGET)

    if(CONFIG_ESP32_NO_BLOBS OR CONFIG_ESP32S2_NO_BLOBS)
        set(link_binary_libs 0)
        set(ldfragments)
    else()
        set(link_binary_libs 1)
        set(ldfragments "linker.lf")
    endif()

    if(IDF_TARGET_ESP32)
        # dport workaround headers are in esp32 component
        set(extra_priv_requires esp32)
    else()
        set(extra_priv_requires)
    endif()

    set(srcs
        "src/coexist.c"
        "src/mesh_event.c"
        "src/smartconfig.c"
        "src/smartconfig_ack.c"
        "src/wifi_init.c"
        "src/wifi_default.c"
        "src/wifi_netif.c"
        "${idf_target}/esp_adapter.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_event esp_phy
                    PRIV_REQUIRES driver esptool_py esp_netif esp_pm esp_timer nvs_flash
                                  wpa_supplicant hal ${extra_priv_requires}
                    LDFRAGMENTS "${ldfragments}")

if(CONFIG_ESP32_WIFI_ENABLED)
    idf_build_get_property(build_dir BUILD_DIR)

    set(target_name "${idf_target}")
    target_link_libraries(${COMPONENT_LIB} PUBLIC "-L \"${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}\"")

    if(link_binary_libs)
        set(blobs coexist core espnow mesh net80211 pp smartconfig wapi)

        foreach(blob ${blobs})
            add_prebuilt_library(${blob} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}/lib${blob}.a"
                                REQUIRES ${COMPONENT_NAME})
            set(blob_reqs ${blobs})
            list(REMOVE_ITEM blob_reqs ${blob}) # remove itself from requirements
            set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${blob_reqs})
            target_link_libraries(${COMPONENT_LIB} PUBLIC ${blob})
        endforeach()
    endif()
endif()
