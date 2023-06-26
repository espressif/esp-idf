idf_component_register(SRCS "rom_test.cpp"
                    INCLUDE_DIRS
                    "."
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES esp_rom)
