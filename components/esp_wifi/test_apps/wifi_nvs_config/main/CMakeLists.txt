idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       PRIV_REQUIRES cmock test_utils nvs_flash esp_wifi esp_event
                       WHOLE_ARCHIVE)

# Create a NVS image from the contents of the `nvs_data` CSV file
# that fits the partition named 'nvs'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
nvs_create_partition_image(nvs ../nvs_data_suffix.csv FLASH_IN_PROJECT)
