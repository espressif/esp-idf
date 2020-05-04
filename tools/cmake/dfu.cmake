# Add DFU build and flashing related targets
#

function(__add_dfu_targets)
    idf_build_get_property(target IDF_TARGET)
    if(NOT "${target}" STREQUAL "esp32s2")
        return()
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)

    add_custom_target(dfu
        COMMAND ${python} ${idf_path}/tools/mkdfu.py write
        -o "${CMAKE_CURRENT_BINARY_DIR}/dfu.bin"
        --json "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json"
        DEPENDS gen_project_binary bootloader
        VERBATIM
        USES_TERMINAL)

    add_custom_target(dfu-flash
        COMMAND dfu-util
        -D "${CMAKE_CURRENT_BINARY_DIR}/dfu.bin"
        VERBATIM
        USES_TERMINAL)
endfunction()
