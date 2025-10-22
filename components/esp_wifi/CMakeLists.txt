idf_build_get_property(idf_target IDF_TARGET)

if(${idf_target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if( NOT CONFIG_ESP_WIFI_ENABLED
    AND NOT CONFIG_ESP_HOST_WIFI_ENABLED
    AND NOT CMAKE_BUILD_EARLY_EXPANSION )
    # No local wifi: provide only netif bindings
    set(srcs
            "src/wifi_default.c"
            "src/wifi_netif.c"
            "src/wifi_default_ap.c")

    # This component provides "esp_wifi" "wifi_apps/nan_app" headers if WiFi not enabled
    # (implementation supported optionally in a managed component esp_wifi_remote)
    idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "wifi_apps/nan_app/include")
    return()
endif()

if(CONFIG_ESP_WIFI_ENABLED OR CONFIG_ESP_HOST_WIFI_ENABLED)

    if(CONFIG_APP_NO_BLOBS)
        set(link_binary_libs 0)
        set(ldfragments)
    else()
        set(link_binary_libs 1)
        set(ldfragments "linker.lf")
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


    list(APPEND srcs "regulatory/esp_wifi_regulatory.c")

    if(CONFIG_ESP_NETIF_USES_TCPIP_WITH_BSD_API AND CONFIG_LWIP_IPV4)
        list(APPEND srcs
            "src/smartconfig_ack.c")
    endif()

    if(CONFIG_ESP_WIFI_NAN_SYNC_ENABLE OR CONFIG_ESP_WIFI_NAN_USD_ENABLE)
        list(APPEND srcs "wifi_apps/nan_app/src/nan_app.c")
    endif()
    if(CONFIG_ESP_WIFI_ENABLE_ROAMING_APP)
        list(APPEND srcs "wifi_apps/roaming_app/src/roaming_app.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "include/local" "wifi_apps/include" "wifi_apps/nan_app/include"

                    REQUIRES esp_event esp_phy esp_netif
                    PRIV_REQUIRES esp_pm esp_timer nvs_flash
                                  wpa_supplicant hal lwip esp_coex
                    PRIV_INCLUDE_DIRS ../wpa_supplicant/src/ ../wpa_supplicant/esp_supplicant/src/
                                  wifi_apps/roaming_app/include wifi_apps/roaming_app/src
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

        # Since some blob names are very generic, add an "esp_wifi_" prefix when creating target names,
        # to avoid conflicts with other libraries.
        set(blob_targets ${blobs})
        list(TRANSFORM blob_targets PREPEND "esp_wifi_")
        foreach(blob ${blobs})
            set(blob_target "esp_wifi_${blob}")
            add_prebuilt_library(${blob_target} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}/lib${blob}.a"
                                REQUIRES ${COMPONENT_NAME})
            set(blob_reqs ${blob_targets})
            list(REMOVE_ITEM blob_reqs ${blob_target}) # remove itself from requirements
            set_property(TARGET ${blob_target} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${blob_reqs})
            target_link_libraries(${COMPONENT_LIB} PUBLIC ${blob_target})
        endforeach()
    endif()
    # TODO IDF-13365: remove the following lines
    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        set_source_files_properties(regulatory/esp_wifi_regulatory.c
                                    PROPERTIES COMPILE_FLAGS
                                    -Wno-unterminated-string-initialization)
    endif()
endif()

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()
