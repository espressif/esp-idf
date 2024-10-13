set(requires "unity"
             "test_utils"
             "esp_driver_uart"
             "esp_driver_gpio"
             "esp_timer"
             "nvs_flash"
             "esp_psram")

set(SRC  "test_app_main.c"
         "test_backtrace.c"
         "test_ipc.c"
         "test_reset_reason.c"
         "test_shared_stack_printf.c"
         "test_stack_check.c"
         "test_system_time.c"
         "test_task_wdt.c")

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED OR CONFIG_SOC_DEEP_SLEEP_SUPPORTED)
    list(APPEND SRC "test_sleep.c")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
    list(APPEND SRC "test_systick_etm.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND SRC "test_delay.c")
endif()

if(CONFIG_ESP_IPC_ISR_ENABLE)
    list(APPEND SRC  "test_ipc_isr.c")
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        list(APPEND SRC  "port/arch/xtensa/test_ipc_isr.S")
    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND SRC  "port/arch/riscv/test_ipc_isr.c")
    endif()
endif()

idf_component_register(SRCS ${SRC}
                    PRIV_INCLUDE_DIRS .
                    PRIV_REQUIRES "${requires}"
                    WHOLE_ARCHIVE)
