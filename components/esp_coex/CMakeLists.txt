idf_build_get_property(idf_target IDF_TARGET)

if(${idf_target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_ESP_COEX_SW_COEXIST_ENABLE OR CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE)
    if(CONFIG_APP_NO_BLOBS)
        set(link_binary_libs 0)
        set(ldfragments)
    else()
        set(link_binary_libs 1)
        set(ldfragments "linker.lf")
    endif()
    set(srcs "src/coexist.c"
             "${idf_target}/esp_coex_adapter.c")
endif()

if(CONFIG_ESP_WIFI_ENABLED)
    list(APPEND srcs "${idf_target}/esp_coex_adapter.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_timer driver esp_event
                    LDFRAGMENTS "${ldfragments}")

if(CONFIG_ESP_COEX_SW_COEXIST_ENABLE OR CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE)
    idf_build_get_property(build_dir BUILD_DIR)

    set(target_name "${idf_target}")
    target_link_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}")

    if(link_binary_libs)
        set(blob coexist)
        add_prebuilt_library(${blob} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}/lib${blob}.a"
                                REQUIRES ${COMPONENT_NAME})
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${blob})
        if(CONFIG_IDF_TARGET_ESP32)
            target_link_libraries(${COMPONENT_LIB} PRIVATE btdm_app)
        endif()
    endif()

    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
