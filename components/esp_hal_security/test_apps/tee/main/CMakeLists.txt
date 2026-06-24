set(srcs "test_main.c" "test_pms.c")
if(CONFIG_SOC_SUPPORT_TEE_INTR_TEST)
    list(APPEND srcs "test_cpu_intr.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ""
                       PRIV_REQUIRES pms_and_cpu_intr esp_psram esp_hal_security
                       WHOLE_ARCHIVE)

# TODO: IDF-14145
if(CONFIG_IDF_TARGET_ESP32C5 OR CONFIG_IDF_TARGET_ESP32C61)
    # Disable zcmp extension to avoid hardware issue with interrupts (DIG-661)
    set(march_option "-march=rv32imac_zicsr_zifencei_zaamo_zalrsc")
    idf_build_set_property(COMPILE_OPTIONS "${march_option}" APPEND)
    idf_build_set_property(LINK_OPTIONS "${march_option}" APPEND)
endif()
