set(COMPONENT_SRCS "pthread.c"
                   "pthread_cond_var.c"
                   "pthread_local_storage.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
register_component()

if(CONFIG_ENABLE_STATIC_TASK_CLEAN_UP_HOOK)
    target_link_libraries(${COMPONENT_LIB} "-Wl,--wrap=vPortCleanUpTCB")
endif()
