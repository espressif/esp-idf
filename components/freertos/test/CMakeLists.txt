idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES cmock test_utils esp_ipc driver)
# Enable task snapshots by setting configENABLE_TASK_SNAPSHOT macro
idf_build_set_property(COMPILE_OPTIONS "-DconfigENABLE_TASK_SNAPSHOT=1" APPEND)
