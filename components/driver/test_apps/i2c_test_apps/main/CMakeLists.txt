set(srcs "test_app_main.c"
         "test_i2c_common.c"
    )

if(CONFIG_SOC_I2C_SUPPORT_SLAVE)
    list(APPEND srcs "test_i2c_multi.c")
    if(CONFIG_I2C_ISR_IRAM_SAFE)
        list(APPEND srcs "test_i2c_iram.c")
    endif()
endif()

if(CONFIG_SOC_I2C_SLAVE_SUPPORT_BROADCAST)
    list(APPEND srcs "test_i2c_broadcast.c")
endif()

if(CONFIG_SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS)
    list(APPEND srcs "test_i2c_ram.c")
endif()

if(CONFIG_SOC_I2C_SUPPORT_10BIT_ADDR AND CONFIG_SOC_I2C_SUPPORT_SLAVE)
    list(APPEND srcs "test_i2c_10bit.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver test_utils
                       WHOLE_ARCHIVE)
