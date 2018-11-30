if(CONFIG_BT_ENABLED OR CMAKE_BUILD_EARLY_EXPANSION)
    set(COMPONENT_SRCDIRS ".")
    set(COMPONENT_ADD_INCLUDEDIRS ".")
    set(COMPONENT_REQUIRES unity nvs_flash bt)

    register_component()
endif()
