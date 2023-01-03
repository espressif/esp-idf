idf_build_get_property(idf_target IDF_TARGET)

if(CONFIG_SW_COEXIST_ENABLE OR CONFIG_EXTERNAL_COEX_ENABLE)
    if(CONFIG_APP_NO_BLOBS)
        set(link_binary_libs 0)
        set(ldfragments)
    else()
        set(link_binary_libs 1)
        set(ldfragments "linker.lf")
    endif()

    set(srcs
        "src/coexist.c"
        "${idf_target}/esp_coex_adapter.c")
endif()

if(CONFIG_ESP32_WIFI_ENABLED)
    list(APPEND srcs "${idf_target}/esp_coex_adapter.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_timer driver esp_event
                    LDFRAGMENTS "${ldfragments}")

if(CONFIG_SW_COEXIST_ENABLE OR CONFIG_EXTERNAL_COEX_ENABLE)
    idf_build_get_property(build_dir BUILD_DIR)

    set(target_name "${idf_target}")
    target_link_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}")

    if(link_binary_libs)
        set(blobs coexist)

        foreach(blob ${blobs})
            add_prebuilt_library(${blob} "${CMAKE_CURRENT_SOURCE_DIR}/lib/${target_name}/lib${blob}.a"
                                REQUIRES ${COMPONENT_NAME})
            set(blob_reqs ${blobs})
            list(REMOVE_ITEM blob_reqs ${blob}) # remove itself from requirements
            set_property(TARGET ${blob} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${blob_reqs})
            target_link_libraries(${COMPONENT_LIB} PUBLIC ${blob})
        endforeach()
    endif()

    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
