set(COMPONENT_SRCS "heap_caps.c"
                   "heap_caps_init.c"
                   "multi_heap.c")

if(NOT CONFIG_HEAP_POISONING_DISABLED)
    list(APPEND COMPONENT_SRCS "multi_heap_poisoning.c")
endif()

if(CONFIG_HEAP_TASK_TRACKING)
    list(APPEND COMPONENT_SRCS "heap_task_info.c")
endif()

if(CONFIG_HEAP_TRACING_STANDALONE)
    list(APPEND COMPONENT_SRCS "heap_trace_standalone.c")
endif()

set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_ADD_LDFRAGMENTS linker.lf)
set(COMPONENT_REQUIRES "")

register_component()

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
        target_link_libraries(${COMPONENT_TARGET} "-Wl,--wrap=${wrap}")
    endforeach()

endif()
