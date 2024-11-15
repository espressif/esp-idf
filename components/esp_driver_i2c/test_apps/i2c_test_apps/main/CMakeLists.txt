set(srcs "test_app_main.c"
         "test_i2c_common.c"
    )

if(CONFIG_SOC_I2C_SUPPORT_SLAVE)
    if(CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2)
        list(APPEND srcs "test_i2c_slave_v2.c")
    else()
        list(APPEND srcs "test_i2c_multi.c")

        if(CONFIG_SOC_I2C_SLAVE_SUPPORT_BROADCAST)
            list(APPEND srcs "test_i2c_broadcast.c")
        endif()

        if(CONFIG_SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS)
            list(APPEND srcs "test_i2c_ram.c")
        endif()

        if(CONFIG_SOC_I2C_SUPPORT_10BIT_ADDR AND CONFIG_SOC_I2C_SUPPORT_SLAVE)
            list(APPEND srcs "test_i2c_10bit.c")
        endif()

        if(CONFIG_SOC_LP_I2C_SUPPORTED)
            list(APPEND srcs "test_lp_i2c.c")
        endif()

        if(CONFIG_SOC_I2C_SUPPORT_SLEEP_RETENTION)
            list(APPEND srcs "test_i2c_sleep_retention.c")
        endif()

        if(CONFIG_I2C_ISR_IRAM_SAFE)
            list(APPEND srcs "test_i2c_iram.c")
        endif()
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver test_utils
                       WHOLE_ARCHIVE)
