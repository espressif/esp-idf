set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS include/freertos)
set(COMPONENT_SRCDIRS ".")
set(COMPONENT_REQUIRES)

register_component()

target_link_libraries(freertos "-Wl,--undefined=uxTopUsedPriority")

set_source_files_properties(
    tasks.c
    event_groups.c
    timers.c
    queue.c
    ringbuf.c
    PROPERTIES COMPILE_DEFINITIONS
    _ESP_FREERTOS_INTERNAL
    )
