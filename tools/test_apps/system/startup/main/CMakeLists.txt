idf_component_register(SRCS "test_startup_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES vfs)

if(CONFIG_SINGLE_CORE_VARIANT)
    target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/chip_info_patch.c")
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_chip_info")
endif()
