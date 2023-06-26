set(sources "test_app_main.c"
            "test_pthread.c"
            "test_pthread_cond_var.c"
            "test_pthread_local_storage.c"
            "test_pthread_cxx.cpp"
            "test_pthread_rwlock.c"
            "test_pthread_semaphore.c")

idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS "."
                       REQUIRES pthread esp_timer test_utils
                       WHOLE_ARCHIVE)
