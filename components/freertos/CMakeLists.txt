set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS include/freertos)
set(COMPONENT_SRCS "FreeRTOS-openocd.c"
                   "croutine.c"
                   "event_groups.c"
                   "list.c"
                   "port.c"
                   "portasm.S"
                   "queue.c"
                   "tasks.c"
                   "timers.c"
                   "xtensa_context.S"
                   "xtensa_init.c"
                   "xtensa_intr.c"
                   "xtensa_intr_asm.S"
                   "xtensa_overlay_os_hook.c"
                   "xtensa_vector_defaults.S"
                   "xtensa_vectors.S")

# app_trace is required by FreeRTOS headers only when CONFIG_SYSVIEW_ENABLE=y,
# but requirements can't depend on config options, so always require it.
set(COMPONENT_REQUIRES app_trace)

register_component()

target_link_libraries(freertos "-Wl,--undefined=uxTopUsedPriority")

set_source_files_properties(
    tasks.c
    event_groups.c
    timers.c
    queue.c
    PROPERTIES COMPILE_DEFINITIONS
    _ESP_FREERTOS_INTERNAL
    )
