set(srcs "basic_freertos_smp_usage.c"
         "create_task_example.c"
         "queue_example.c"
         "lock_example.c"
         "task_notify_example.c"
         "batch_processing_example.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES console esp_timer)
