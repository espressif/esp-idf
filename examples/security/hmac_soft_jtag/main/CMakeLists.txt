idf_component_register(SRCS "example_main.c" "jtag_commands.c"
                       PRIV_REQUIRES console efuse mbedtls nvs_flash
                       PRIV_INCLUDE_DIRS ".")
