# CI build overlay: NimBLE host (sdkconfig.defaults is NimBLE-first).
# Explicit config so idf-build-apps builds both nimble and bluedroid in CI.

CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_BLUEDROID_ENABLED=n
CONFIG_BT_NIMBLE_SM_SC=y
CONFIG_BT_NIMBLE_NVS_PERSIST=y
