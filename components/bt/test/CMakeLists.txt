if(CONFIG_BT_ENABLED)
    set(COMPONENT_SRCDIRS ".")
    set(COMPONENT_ADD_INCLUDEDIRS ".")
endif()

set(COMPONENT_REQUIRES unity nvs_flash bt)

register_component()
