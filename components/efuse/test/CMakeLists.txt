idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "." "include"
                       PRIV_INCLUDE_DIRS "../private_include"
                       PRIV_REQUIRES cmock test_utils efuse bootloader_support
                       )
