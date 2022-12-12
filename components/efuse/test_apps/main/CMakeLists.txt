idf_build_get_property(target IDF_TARGET)

if(CONFIG_SOC_EFUSE_KEY_PURPOSE_FIELD)
    set(dir "with_key_purposes")
else()
    if(CONFIG_SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK)
        set(dir "one_key_block")
    else()
        set(dir "three_key_blocks")
    endif()
endif()

set(src_dirs "." "${dir}")

idf_component_register(SRC_DIRS "${src_dirs}"
                       PRIV_INCLUDE_DIRS "." "${dir}/include" "../../private_include" "../../${target}/private_include"
                       PRIV_REQUIRES cmock efuse bootloader_support esp_timer
                       WHOLE_ARCHIVE)
