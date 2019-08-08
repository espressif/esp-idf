set(srcs 
    "heap_caps.c"
    "heap_caps_init.c"
    "multi_heap.c")

if(NOT CONFIG_HEAP_POISONING_DISABLED)
    list(APPEND srcs "multi_heap_poisoning.c")
endif()

if(CONFIG_HEAP_TASK_TRACKING)
    list(APPEND srcs "heap_task_info.c")
endif()

if(CONFIG_HEAP_TRACING_STANDALONE)
    list(APPEND srcs "heap_trace_standalone.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES soc)

if(CONFIG_HEAP_TRACING)
    set(WRAP_FUNCTIONS
        calloc
        malloc
        free
        realloc
        heap_caps_malloc
        heap_caps_free
        heap_caps_realloc
        heap_caps_malloc_default
        heap_caps_realloc_default)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()

endif()
