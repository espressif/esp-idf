idf_component_register(SRCS "pthread.c"
                            "pthread_cond_var.c"
                            "pthread_local_storage.c"
                    INCLUDE_DIRS include)

if(GCC_NOT_5_2_0)
    set(extra_link_flags "-u pthread_include_pthread_impl")
    list(APPEND extra_link_flags "-u pthread_include_pthread_cond_impl")
    list(APPEND extra_link_flags "-u pthread_include_pthread_local_storage_impl")
endif()

if(CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP)
    target_link_libraries(${COMPONENT_LIB} "-Wl,--wrap=vPortCleanUpTCB")
endif()

if(extra_link_flags)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "${extra_link_flags}")
endif()
