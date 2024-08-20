idf_component_register(SRCS "test_psram_no_malloc_task_stack.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_psram freertos heap)
