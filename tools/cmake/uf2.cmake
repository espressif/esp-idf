# Add UF2 build target

function(__add_uf2_targets)
    idf_build_get_property(target IDF_TARGET)
    if("${target}" STREQUAL "esp32")
        set(uf2_family_id "0x1c5f21b0")
    elseif("${target}" STREQUAL "esp32s2")
        set(uf2_family_id "0xbfdd4eee")
    elseif("${target}" STREQUAL "esp32c3")
        set(uf2_family_id "0xd42ba06c")
    elseif("${target}" STREQUAL "esp32s3")
        set(uf2_family_id "0xc47e5767")
    elseif("${target}" STREQUAL "esp32h2")
        set(uf2_family_id "0x332726f6")
    elseif("${target}" STREQUAL "esp32c2")
        set(uf2_family_id "0x2b88d29c")
    elseif("${target}" STREQUAL "esp32c6") # TODO: IDF-5626
        set(uf2_family_id "0x2b88d29c")
    elseif("${target}" STREQUAL "linux")
        return()
    else()
        message(FATAL_ERROR "UF2 family identificator is unknown for ${target}")
        # Generate an ID and submit a pull request as described here: https://github.com/microsoft/uf2
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)

    add_custom_target(uf2-app
        COMMAND ${python} ${idf_path}/tools/mkuf2.py write
        -o "${CMAKE_CURRENT_BINARY_DIR}/uf2-app.bin"
        --json "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json"
        --chip-id "${uf2_family_id}"
        --bin app
        DEPENDS gen_project_binary
        VERBATIM
        USES_TERMINAL)

    add_custom_target(uf2
        COMMAND ${python} ${idf_path}/tools/mkuf2.py write
        -o "${CMAKE_CURRENT_BINARY_DIR}/uf2.bin"
        --json "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json"
        --chip-id "${uf2_family_id}"
        DEPENDS gen_project_binary bootloader
        VERBATIM
        USES_TERMINAL)
endfunction()
