idf_build_get_property(idf_target IDF_TARGET)

if(${idf_target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_ESP_WIFI_ENABLED OR CONFIG_ESP_HOST_WIFI_ENABLED)

    if(CONFIG_APP_NO_BLOBS)
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
        "src/lib_printf.c"
        "src/mesh_event.c"
        "src/smartconfig.c"
        "src/wifi_init.c"
        "src/wifi_default.c"
        "src/wifi_netif.c"
        "src/wifi_default_ap.c"
        "${idf_target}/esp_adapter.c")

    if(CONFIG_ESP_NETIF_USES_TCPIP_WITH_BSD_API AND CONFIG_LWIP_IPV4)
        list(APPEND srcs
            "src/smartconfig_ack.c")
    endif()

    if(CONFIG_ESP_WIFI_NAN_ENABLE)
        list(APPEND srcs "wifi_apps/nan_app/src/nan_app.c")
    endif()
    if(CONFIG_ESP_WIFI_ENABLE_ROAMING_APP)
        list(APPEND srcs "wifi_apps/roaming_app/src/roaming_app.c")
    endif()
    set(local_include_dirs include/local)
else()
    # No local wifi: provide only netif bindings
    set(srcs
            "src/wifi_default.c"
            "src/wifi_netif.c"
            "src/wifi_default_ap.c")
    set(local_include_dirs)

endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "wifi_apps/include" "wifi_apps/nan_app/include" ${local_include_dirs}

                    REQUIRES esp_event esp_phy esp_netif
                    PRIV_REQUIRES driver esptool_py esp_pm esp_timer nvs_flash
                                  wpa_supplicant hal lwip esp_coex ${extra_priv_requires}
                    PRIV_INCLUDE_DIRS ../wpa_supplicant/src/ ../wpa_supplicant/esp_supplicant/src/
                                  wifi_apps/roaming_app/include
                    LDFRAGMENTS "${ldfragments}")

if(CONFIG_ESP_WIFI_ENABLED OR CONFIG_ESP_HOST_WIFI_ENABLED)
    idf_build_get_property(build_dir BUILD_DIR)

    if(CONFIG_ESP_HOST_WIFI_ENABLED)
        set(target_name "${CONFIG_ESP_WIFI_CONTROLLER_TARGET}_host")
    else()
        set(target_name "${idf_target}")
    endif()

    target_link_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}")

    if(link_binary_libs)
        if(CONFIG_ESP_HOST_WIFI_ENABLED)
            set(blobs core espnow net80211 target smartconfig wapi)
        else()
            if(CONFIG_IDF_TARGET_ESP32C2)
                set(blobs core espnow net80211 pp smartconfig)
            else()
                set(blobs core espnow mesh net80211 pp smartconfig wapi)
            endif()
        endif()

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

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()
