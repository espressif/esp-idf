idf_component_register(SRCS "sbv2_multisig_app_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES efuse bootloader_support
                    REQUIRED_IDF_TARGETS esp32s3)
