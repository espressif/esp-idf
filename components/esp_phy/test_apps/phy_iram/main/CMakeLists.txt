set(srcs "test_app_main.c"
         "test_phy.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity nvs_flash esp_phy ieee802154
                       WHOLE_ARCHIVE)
