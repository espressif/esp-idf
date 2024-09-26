idf_component_register(SRCS "nvs_page_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
                    PRIV_INCLUDE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../private_include"
                    REQUIRES nvs_flash
                    PRIV_REQUIRES spi_flash)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} --coverage)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)
endif()
