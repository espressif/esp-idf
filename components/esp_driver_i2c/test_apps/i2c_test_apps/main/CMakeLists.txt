set(srcs "test_app_main.c"
         "test_i2c_common.cpp"
    )

if(CONFIG_SOC_I2C_SUPPORT_SLAVE AND CONFIG_SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE)
    list(APPEND srcs "test_i2c_multi.cpp")

    if(CONFIG_SOC_I2C_SLAVE_SUPPORT_BROADCAST)
        list(APPEND srcs "test_i2c_broadcast.cpp")
    endif()

    if(CONFIG_SOC_I2C_SUPPORT_10BIT_ADDR)
        list(APPEND srcs "test_i2c_10bit.cpp")
    endif()

    if(CONFIG_SOC_LP_I2C_SUPPORTED)
        list(APPEND srcs "test_lp_i2c.cpp")
    endif()

    if(CONFIG_SOC_I2C_SUPPORT_SLEEP_RETENTION AND CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
       list(APPEND srcs "test_i2c_sleep_retention.cpp")
    endif()

    if(CONFIG_I2C_ISR_IRAM_SAFE)
       list(APPEND srcs "test_i2c_iram.cpp")
    endif()
endif()

if(NOT CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM)
    list(APPEND srcs "test_i2c_flash_text.cpp")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_i2c unity esp_driver_uart test_utils esp_pm esp_driver_gptimer
                       WHOLE_ARCHIVE)
