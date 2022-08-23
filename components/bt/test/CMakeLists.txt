if(CONFIG_BT_ENABLED OR CMAKE_BUILD_EARLY_EXPANSION)
    idf_component_register(SRC_DIRS "."
                        PRIV_INCLUDE_DIRS "."
                        PRIV_REQUIRES cmock nvs_flash bt esp_ringbuf)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
