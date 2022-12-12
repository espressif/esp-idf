set(sources "pthread.c"
            "pthread_cond_var.c"
            "pthread_local_storage.c"
            "pthread_rwlock.c")

idf_component_register(SRCS ${sources}
                    INCLUDE_DIRS include)

idf_build_set_property(COMPILE_DEFINITIONS "_POSIX_READER_WRITER_LOCKS" APPEND)

set(extra_link_flags "-u pthread_include_pthread_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_cond_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_local_storage_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_rwlock_impl")

if(extra_link_flags)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "${extra_link_flags}")
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
