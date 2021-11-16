if(BOOTLOADER_BUILD)
    # bootloader only needs FreeRTOS for config, not for anything else
    idf_component_register()
    return()
endif()

idf_build_get_property(target IDF_TARGET)

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    set(srcs
        "FreeRTOS-Kernel/portable/xtensa/port.c"
        "FreeRTOS-Kernel/portable/xtensa/portasm.S"
        "FreeRTOS-Kernel/portable/xtensa/xtensa_context.S"
        "FreeRTOS-Kernel/portable/xtensa/xtensa_init.c"
        "FreeRTOS-Kernel/portable/xtensa/xtensa_overlay_os_hook.c"
        "FreeRTOS-Kernel/portable/xtensa/xtensa_vector_defaults.S"
        "FreeRTOS-Kernel/portable/xtensa/xtensa_vectors.S")

    set(include_dirs
        FreeRTOS-Kernel/include
        esp_additions/include/freertos              # For files with #include "FreeRTOSConfig.h"
        FreeRTOS-Kernel/portable/xtensa/include     # For arch-specific FreeRTOSConfig_arch.h in portable/<arch>/include
        esp_additions/include)                      # For files with #include "freertos/FreeRTOSConfig.h"

    set(private_include_dirs
        FreeRTOS-Kernel/portable/xtensa/include/freertos
        FreeRTOS-Kernel/portable/xtensa
        FreeRTOS-Kernel/portable/priv_include
        .)

elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    set(srcs
        "FreeRTOS-Kernel/portable/riscv/port.c"
        "FreeRTOS-Kernel/portable/riscv/portasm.S")

    set(include_dirs
        FreeRTOS-Kernel/include
        esp_additions/include/freertos              # For files with #include "FreeRTOSConfig.h"
        FreeRTOS-Kernel/portable/riscv/include      # For arch-specific FreeRTOSConfig_arch.h in portable/<arch>/include
        esp_additions/include)                      # For files with #include "freertos/FreeRTOSConfig.h"

    set(private_include_dirs
        FreeRTOS-Kernel/portable/riscv/include/freertos
        FreeRTOS-Kernel/portable/riscv
        FreeRTOS-Kernel/portable/priv_include
        .)

endif()

list(APPEND srcs
    "esp_additions/task_snapshot.c"
    "FreeRTOS-Kernel/portable/port_common.c"
    "FreeRTOS-Kernel/portable/port_systick.c"
    "FreeRTOS-Kernel/croutine.c"
    "FreeRTOS-Kernel/event_groups.c"
    "FreeRTOS-Kernel/list.c"
    "FreeRTOS-Kernel/queue.c"
    "FreeRTOS-Kernel/tasks.c"
    "FreeRTOS-Kernel/timers.c"
    "FreeRTOS-Kernel/stream_buffer.c"
    "FreeRTOS-openocd.c"
    "esp_additions/freertos_v8_compat.c")

list(APPEND private_include_dirs
    "FreeRTOS-Kernel/include/freertos")

if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    list(APPEND srcs "FreeRTOS-Kernel/portable/xtensa/xtensa_loadstore_handler.S")
endif()

# esp_timer is required by FreeRTOS because we use esp_tiemr_get_time() to do profiling
# app_trace is required by FreeRTOS headers only when CONFIG_APPTRACE_SV_ENABLE=y,
# REQUIRES can't depend on config options, so always require it.
set(required_components app_trace esp_timer)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS  ${private_include_dirs}
                    LDFRAGMENTS linker.lf
                    REQUIRES ${required_components}
                    PRIV_REQUIRES soc esp_pm)

idf_component_get_property(COMPONENT_DIR freertos COMPONENT_DIR)
idf_component_set_property(freertos ORIG_INCLUDE_PATH "${COMPONENT_DIR}/include/freertos/")

if(CONFIG_FREERTOS_DEBUG_OCDAWARE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=uxTopUsedPriority")
endif()

set_source_files_properties(
    tasks.c
    event_groups.c
    timers.c
    queue.c
    stream_buffer.c
    PROPERTIES COMPILE_DEFINITIONS
    _ESP_FREERTOS_INTERNAL
    )

# The freertos component provides the `start_app` and `start_app_other_cores`
# if it is included in the build. It then calls `app_main`
# from the main task created, which must be provided by the user.
# Like for `start_app` and `start_app_other_cores`,
# we can't establish dependency on what we don't yet know, so we force the
# linker to not drop this symbol.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_main")
