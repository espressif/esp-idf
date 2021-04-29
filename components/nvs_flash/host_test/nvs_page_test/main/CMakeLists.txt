idf_component_register(SRCS "nvs_page_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
                    REQUIRES cmock nvs_flash spi_flash)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} --coverage)
