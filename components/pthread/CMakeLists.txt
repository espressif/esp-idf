idf_component_register(SRCS "pthread.c"
                            "pthread_cond_var.c"
                            "pthread_local_storage.c"
                    INCLUDE_DIRS include)

if(CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP)
    target_link_libraries(${COMPONENT_LIB} "-Wl,--wrap=vPortCleanUpTCB")
endif()
