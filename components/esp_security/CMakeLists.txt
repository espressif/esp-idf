if(NOT BOOTLOADER_BUILD)
    set(src_dirs "src")
endif()
idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS "include")
