idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "esp32h2")
    return()
endif()

if(NOT BOOTLOADER_BUILD)
    # [refactor-todo] propagate these requirements for compatibility
    # remove in the future
    set(legacy_reqs driver efuse soc)
endif()

idf_component_register(REQUIRES riscv "${legacy_reqs}"
                       REQUIRED_IDF_TARGETS esp32h2)
