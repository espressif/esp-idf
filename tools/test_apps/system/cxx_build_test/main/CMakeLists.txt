idf_component_register(SRCS cxx_build_test_main.cpp
                            test_soc_reg_macros.cpp
                            test_esp_hw_support.cpp
                            test_cxx_standard.cpp
                            test_lcd.cpp
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_lcd
                       REQUIRES soc)

set_source_files_properties(test_esp_hw_support.cpp PROPERTIES COMPILE_FLAGS -Wsign-conversion)
