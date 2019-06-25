if(CONFIG_BT_ENABLED OR CMAKE_BUILD_EARLY_EXPANSION)
    idf_component_register(SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES unity nvs_flash bt)
endif()