if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp_common is headers
    set(COMPONENT_ADD_INCLUDEDIRS include)
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS )
    register_component()
else()
    # Regular app build
    set(COMPONENT_SRCS
        "src/dbg_stubs.c"
        "src/esp_err_to_name.c"
        "src/esp_timer.c"
        "src/ets_timer_legacy.c"
        "src/pm_locks.c"
        "src/stack_check.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS)
    set(COMPONENT_REQUIRES)
    # dbg_stubs.c needs eri.h from xtensa-debug-module
    set(COMPONENT_PRIV_REQUIRES "xtensa-debug-module")
    register_component()

    set_source_files_properties(
        "src/stack_check.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
endif()
