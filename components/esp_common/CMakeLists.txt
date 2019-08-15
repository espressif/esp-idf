if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp_common is headers
    idf_component_register(INCLUDE_DIRS include)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
else()
    # Regular app build
    set(srcs "src/dbg_stubs.c"
             "src/esp_err_to_name.c"
             "src/esp_timer.c"
             "src/ets_timer_legacy.c"
             "src/freertos_hooks.c"
             "src/pm_locks.c"
             "src/stack_check.c")

    # IPC framework is not applicable if freertos unicore config is selected
    if(NOT CONFIG_FREERTOS_UNICORE)
        list(APPEND srcs "src/ipc.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES soc)

    set_source_files_properties(
        "src/stack_check.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_LIBRARIES "-Wl,--gc-sections")
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
endif()
