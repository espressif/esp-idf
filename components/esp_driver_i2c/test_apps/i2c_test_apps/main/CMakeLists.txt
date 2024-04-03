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

if(CONFIG_SOC_LP_I2C_SUPPORTED)
    list(APPEND srcs "test_lp_i2c.c")
endif()

# Only build this file with `CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP` and `CONFIG_IEEE802154_ENABLED` enabled
# Enable `CONFIG_IEEE802154_ENABLED` is for modem domain really power down.
# This reliable can be removed if the sleep retention got finished.
if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP AND CONFIG_IEEE802154_ENABLED)
    list(APPEND srcs "test_i2c_sleep_retention.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver test_utils ieee802154
                       WHOLE_ARCHIVE)
