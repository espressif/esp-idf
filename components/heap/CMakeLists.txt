set(COMPONENT_SRCS "heap_caps.c"
                   "heap_caps_init.c"
                   "heap_trace.c"
                   "multi_heap.c")

if(NOT CONFIG_HEAP_POISONING_DISABLED)
    set(COMPONENT_SRCS ${COMPONENT_SRCS} multi_heap_poisoning.c)
endif()

set(COMPONENT_ADD_INCLUDEDIRS "include")

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
        target_link_libraries(heap "-Wl,--wrap=${wrap}")
    endforeach()

endif()
