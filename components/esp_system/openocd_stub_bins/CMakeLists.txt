idf_build_get_property(target IDF_TARGET)

# Function to add custom commands for copying stub files
function(add_copy_command src dest)
    add_custom_command(
        OUTPUT ${dest}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${dest}
        DEPENDS ${src}
        COMMENT "Copying ${src} to ${dest}"
        VERBATIM
    )
endfunction()

set(openocd_path $ENV{OPENOCD_SCRIPTS})

if(openocd_path)
    set(stub_bin_path ${openocd_path}/../espressif/stub_bins)
    if(IS_DIRECTORY ${stub_bin_path} AND IS_DIRECTORY ${stub_bin_path}/${target})
        set(code_bin "${stub_bin_path}/${target}/stub_flash_idf_binary_code.inc")
        set(data_bin "${stub_bin_path}/${target}/stub_flash_idf_binary_data.inc")
        set(img_header "${stub_bin_path}/${target}/stub_flash_idf_image.h")
        if(EXISTS ${code_bin} AND EXISTS ${data_bin} AND EXISTS ${img_header})
            set(dest_dir "${CMAKE_BINARY_DIR}/openocd_stub_bins")
            set(output_code_bin "${dest_dir}/stub_flash_idf_binary_code.inc")
            set(output_data_bin "${dest_dir}/stub_flash_idf_binary_data.inc")
            set(output_img_header "${dest_dir}/stub_flash_idf_image.h")
            add_copy_command(${code_bin} ${output_code_bin})
            add_copy_command(${data_bin} ${output_data_bin})
            add_copy_command(${img_header} ${output_img_header})
            add_custom_target(copy_stub_bins ALL
                DEPENDS ${output_code_bin} ${output_data_bin} ${output_img_header}
                COMMENT "Copying OpenOCD stub binaries and image header"
            )
            add_dependencies(${COMPONENT_LIB} copy_stub_bins)
        endif()
    else()
        message(FATAL_ERROR
            "OpenOCD stub binary files couldn't be found! "
            "To bypass this error, disable the CONFIG_ESP_DEBUG_INCLUDE_OCD_STUB_BINS option")
    endif()

    set(srcs "openocd_stub_flasher.c")
    add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")
    target_sources(${COMPONENT_LIB} PRIVATE ${srcs})
    target_include_directories(${COMPONENT_LIB} PRIVATE ${dest_dir})
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_system_include_openocd_stub_binaries")

endif()
