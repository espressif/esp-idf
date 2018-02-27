set(COMPONENT_SRCDIRS "src")

if(${BOOTLOADER_BUILD})
    set(COMPONENT_ADD_INCLUDEDIRS "include include_priv")
else()
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS "include_priv")
endif()

register_component()
