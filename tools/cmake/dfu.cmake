# Add DFU build and flashing related targets
#

function(__add_dfu_targets)
    idf_build_get_property(target IDF_TARGET)
    if("${target}" STREQUAL "esp32")
        return()
    elseif("${target}" STREQUAL "esp32s2")
        set(dfu_pid "2")
    elseif("${target}" STREQUAL "esp32s3")
        set(dfu_pid "9")
    elseif("${target}" STREQUAL "esp32c3")
        return()
    elseif("${target}" STREQUAL "esp32c2")
        return()
    elseif("${target}" STREQUAL "esp32c6")
        return()
    elseif("${target}" STREQUAL "esp32c61")
        return()
    elseif("${target}" STREQUAL "esp32c5")
        return()
    elseif("${target}" STREQUAL "esp32h2")
        return()
    elseif("${target}" STREQUAL "esp32p4")
        set(dfu_pid "12")
    elseif("${target}" STREQUAL "linux")
        return()
    else()
        message(FATAL_ERROR "DFU PID unknown for ${target}")
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)

    add_custom_target(dfu
        COMMAND ${python} ${idf_path}/tools/mkdfu.py write
        -o "${CMAKE_CURRENT_BINARY_DIR}/dfu.bin"
        --json "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json"
        --pid "${dfu_pid}"
        --flash-size "${CONFIG_ESPTOOLPY_FLASHSIZE}"
        DEPENDS gen_project_binary bootloader
        VERBATIM
        USES_TERMINAL)

    add_custom_target(dfu-list
        COMMAND ${CMAKE_COMMAND}
        -D ESP_DFU_LIST="1"
        -P ${idf_path}/tools/cmake/run_dfu_util.cmake
        USES_TERMINAL)

    add_custom_target(dfu-flash
        COMMAND ${CMAKE_COMMAND}
        -D ESP_DFU_BIN="${CMAKE_CURRENT_BINARY_DIR}/dfu.bin"
        -D ESP_DFU_PID="${dfu_pid}"
        -P ${idf_path}/tools/cmake/run_dfu_util.cmake
        USES_TERMINAL)
endfunction()
