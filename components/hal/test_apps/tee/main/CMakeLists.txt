set(srcs "test_main.c" "test_pms.c")
if(CONFIG_SOC_SUPPORT_TEE_INTR_TEST)
    list(APPEND srcs "test_cpu_intr.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ""
                       PRIV_REQUIRES pms_and_cpu_intr esp_psram
                       WHOLE_ARCHIVE)
