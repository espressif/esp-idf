message(STATUS "building ESP_PARTITION MOCKS")

idf_component_get_property(original_esp_partition_dir esp_partition COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_esp_partition_dir}/include"
    REQUIRES spi_flash
    MOCK_HEADER_FILES
    ${original_esp_partition_dir}/include/esp_partition.h)
