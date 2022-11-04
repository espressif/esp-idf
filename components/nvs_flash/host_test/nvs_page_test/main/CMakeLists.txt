idf_component_register(SRCS "nvs_page_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
                    PRIV_INCLUDE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../private_include"
                    REQUIRES cmock nvs_flash spi_flash partition_table esp_partition)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} --coverage)
