# NimBLE host overlay for this example.
# Build it as described in README.md, with its own build directory and sdkconfig.

CONFIG_BT_BLUEDROID_ENABLED=n
CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_NIMBLE_EXT_ADV=y
CONFIG_BT_NIMBLE_ISO=y
CONFIG_BT_NIMBLE_LOG_LEVEL_WARNING=y
