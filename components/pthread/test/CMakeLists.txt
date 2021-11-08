set(sources "test_pthread.c"
            "test_pthread_cond_var.c"
            "test_pthread_local_storage.c"
            "test_pthread_cxx.cpp"
            "test_pthread_rwlock.c")

idf_component_register(SRCS ${sources}
                    PRIV_REQUIRES cmock test_utils pthread)
