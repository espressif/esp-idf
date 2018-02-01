register_config_only_component()

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/flash_project_args.in"
  "${CMAKE_BINARY_DIR}/flash_project_args"
  )
if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
  file_append_line(${CMAKE_BINARY_DIR}/flash_project_args
    "${CONFIG_PHY_DATA_OFFSET} esp32/phy_init_data.bin")
endif()
