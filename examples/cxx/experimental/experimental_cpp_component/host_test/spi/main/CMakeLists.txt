idf_component_get_property(cpp_component experimental_cpp_component COMPONENT_DIR)

idf_component_register(SRCS "spi_cxx_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "${cpp_component}/host_test/fixtures"
                    "${cpp_component}/private_include"
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES cmock driver experimental_cpp_component)
