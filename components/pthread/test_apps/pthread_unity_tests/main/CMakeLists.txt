idf_build_get_property(target IDF_TARGET)

set(sources "test_app_main.c" "test_esp_pthread.c")
set(priv_requires "pthread" "unity")

if(NOT ${target} STREQUAL "linux")
    list(APPEND sources "test_pthread.c"
                        "test_pthread_cond_var.c"
                        "test_pthread_local_storage.c"
                        "test_pthread_cxx.cpp"
                        "test_pthread_rwlock.c"
                        "test_pthread_semaphore.c")
    list(APPEND priv_requires "esp_timer" "test_utils")
endif()

idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS "."
                       REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)
