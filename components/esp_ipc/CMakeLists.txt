set(srcs "src/esp_ipc.c")

if(CONFIG_ESP_IPC_ISR_ENABLE)
    list(APPEND srcs "src/esp_ipc_isr/esp_ipc_isr.c"
                    "src/esp_ipc_isr/esp_ipc_isr_handler.S"
                    "src/esp_ipc_isr/esp_ipc_isr_routines.S")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include")
