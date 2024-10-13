set(srcs "test_app_main.c"
         "test_i2c.c"
    )

# Only build this file with `CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP` and `CONFIG_IEEE802154_ENABLED` enabled
# Enable `CONFIG_IEEE802154_ENABLED` is for modem domain really power down.
# This reliable can be removed if the sleep retention got finished.
if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP AND CONFIG_IEEE802154_ENABLED)
    list(APPEND srcs "test_legacy_i2c_sleep_retention.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils driver ieee802154
                       WHOLE_ARCHIVE)
