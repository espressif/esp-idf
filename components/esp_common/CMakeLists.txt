if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp_common is headers
    set(COMPONENT_ADD_INCLUDEDIRS include)
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS )
    register_component()
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
else()
    # Regular app build
    set(COMPONENT_SRCS
        "src/dbg_stubs.c"
        "src/esp_err_to_name.c"
        "src/esp_timer.c"
        "src/ets_timer_legacy.c"
        "src/freertos_hooks.c"
        "src/ipc.c"
        "src/pm_locks.c"
        "src/stack_check.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS)
    set(COMPONENT_REQUIRES)
    set(COMPONENT_PRIV_REQUIRES soc)

    register_component()

    set_source_files_properties(
        "src/stack_check.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_LIBRARIES "-Wl,--gc-sections")
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
endif()
