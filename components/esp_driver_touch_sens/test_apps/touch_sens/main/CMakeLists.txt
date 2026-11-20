set(srcs "test_app_main.c" "test_touch_sens_common.cpp")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES unity esp_driver_touch_sens esp_psram
                    REQUIRES ulp
                    WHOLE_ARCHIVE)

if(CONFIG_IDF_TARGET_ESP32)
    set(ulp_sources "ulp_fsm/main.S")
elseif(CONFIG_IDF_TARGET_ESP32S2 OR CONFIG_IDF_TARGET_ESP32S3)
    set(ulp_sources "ulp_rv/main.c")
elseif(CONFIG_IDF_TARGET_ESP32P4 OR CONFIG_IDF_TARGET_ESP32S31)
    set(ulp_sources "ulp_lpc/main.c")
endif()

if(ulp_sources)
    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_exp_dep_srcs "test_touch_sens_common.cpp")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
endif()
