idf_component_get_property(spi_flash_dir spi_flash COMPONENT_DIR)
idf_component_get_property(cpp_component experimental_cpp_component COMPONENT_DIR)

idf_component_register(SRCS "gpio_cxx_test.cpp"
                    "${cpp_component}/esp_exception.cpp"
                    "${cpp_component}/gpio_cxx.cpp"
                    INCLUDE_DIRS
                    "."
                    "${cpp_component}/host_test/fixtures"
                    "${cpp_component}/include"
                    "${cpp_component}/test" # FIXME for unity_cxx.hpp, make it generally available instead
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES driver cmock)
