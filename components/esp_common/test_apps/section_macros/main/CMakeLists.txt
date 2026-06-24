idf_component_register(SRCS "test_section_macros.c" "test_entries_a.c" "test_entries_b.c"
                       INCLUDE_DIRS "."
                       WHOLE_ARCHIVE)

if(CONFIG_IDF_TARGET_LINUX AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # Add linker script that collects the .test_data_table section
    # On macOS, custom sections are handled via Mach-O __DATA segments
    # and do not require linker scripts (which use GNU ld -T syntax).
    target_link_options(${COMPONENT_LIB} INTERFACE
        "-Wl,-T,${CMAKE_CURRENT_LIST_DIR}/../ld/test_section.ld")
endif()
