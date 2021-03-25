idf_build_get_property(target IDF_TARGET)

idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "." "include" "../private_include" ../${target}/private_include
                       PRIV_REQUIRES cmock test_utils efuse bootloader_support
                       )
