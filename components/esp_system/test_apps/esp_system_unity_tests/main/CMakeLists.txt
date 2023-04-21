set(requires "unity"
             "test_utils"
             "driver"
             "esp_timer"
             "nvs_flash"
             "esp_psram")

set(SRC  "test_app_main.c"
         "test_backtrace.c"
         "test_delay.c"
         "test_ipc_isr.c"
         "test_ipc_isr.S"
         "test_ipc.c"
         "test_reset_reason.c"
         "test_sleep.c"
         "test_stack_check.c"
         "test_system_time.c"
         "test_task_wdt.c")

idf_component_register(SRCS ${SRC}
                    PRIV_INCLUDE_DIRS .
                    PRIV_REQUIRES "${requires}"
                    WHOLE_ARCHIVE)
