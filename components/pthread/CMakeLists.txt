idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    # Make pthread component an empty interface lib referencing host pthread for Linux target
    idf_component_register()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${COMPONENT_LIB} INTERFACE Threads::Threads)
    return()
endif()

set(sources "pthread.c"
            "pthread_cond_var.c"
            "pthread_local_storage.c"
            "pthread_rwlock.c"
            "pthread_semaphore.c")

idf_component_register(SRCS ${sources}
                    INCLUDE_DIRS include)

idf_build_set_property(COMPILE_DEFINITIONS "_POSIX_READER_WRITER_LOCKS" APPEND)

set(extra_link_flags "-u pthread_include_pthread_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_cond_var_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_local_storage_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_rwlock_impl")
list(APPEND extra_link_flags "-u pthread_include_pthread_semaphore_impl")

if(extra_link_flags)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "${extra_link_flags}")
endif()
