register_config_only_component()

# Generate pre-canned flasher args files suitable for passing to esptool.py
foreach(part project app bootloader partition_table)
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/flash_${part}_args.in"
        "${CMAKE_BINARY_DIR}/flash_${part}_args"
        )
endforeach()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/flasher_args.json.in"
    "${CMAKE_BINARY_DIR}/flasher_args.json"
    )
