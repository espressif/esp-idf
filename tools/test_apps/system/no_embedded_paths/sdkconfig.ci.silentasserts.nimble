CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT=y
CONFIG_COMPILER_HIDE_PATHS_MACROS=n

# the other sdkconfig builds Bluedroid, build NimBLE here
#
# (Note: ESP32-S2 will build both these configs as well, but they're identical. This is simpler than
# needing to specify per-target configs for both Bluedroid and Nimble on ESP32, ESP32-C3.)
CONFIG_BT_ENABLED=y
CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS=n
CONFIG_BLE_MESH=y
CONFIG_BT_NIMBLE_MAX_CONNECTIONS=1
