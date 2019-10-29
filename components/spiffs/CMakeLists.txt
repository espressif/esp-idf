idf_component_register(SRCS "esp_spiffs.c"
                            "spiffs_api.c"
                            "spiffs/src/spiffs_cache.c"
                            "spiffs/src/spiffs_check.c"
                            "spiffs/src/spiffs_gc.c"
                            "spiffs/src/spiffs_hydrogen.c"
                            "spiffs/src/spiffs_nucleus.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "." "spiffs/src"
                    REQUIRES spi_flash
                    PRIV_REQUIRES bootloader_support)

set_source_files_properties(spiffs/src/spiffs_nucleus.c PROPERTIES COMPILE_FLAGS -Wno-stringop-truncation)

