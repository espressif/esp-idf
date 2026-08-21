idf_component_register(SRCS test_spiffs.c
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES spiffs unity vfs esp_psram
                       WHOLE_ARCHIVE
                      )
