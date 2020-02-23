idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." "include"
                       PRIV_INCLUDE_DIRS "../private_include"
                       REQUIRES unity test_utils efuse bootloader_support
                       )