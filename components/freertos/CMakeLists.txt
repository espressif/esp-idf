if(BOOTLOADER_BUILD)
    # bootloader only needs FreeRTOS for config, not for anything else
    idf_component_register()
    return()
endif()

idf_build_get_property(target IDF_TARGET)

if(CONFIG_FREERTOS_SMP)
    set(ldfragments linker_smp.lf)
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        set(srcs
            "FreeRTOS-Kernel-SMP/portable/xtensa/port.c"
            "FreeRTOS-Kernel-SMP/portable/xtensa/portasm.S"
            "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_context.S"
            "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_init.c"
            "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_overlay_os_hook.c"
            "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_vector_defaults.S"
            "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_vectors.S")

        set(include_dirs
            "FreeRTOS-Kernel-SMP/include" # FreeRTOS headers via #include "freertos/xxx.h"
            "esp_additions/include/freertos" # For config via #include "FreeRTOSConfig.h"
            "FreeRTOS-Kernel-SMP/portable/xtensa/include" # Xtensa/Arch Config headers via #include "freertos/xxx.h"
            "esp_additions/include" # For #include "freertos/task_snapshot.h" and #include "freertos/FreeRTOSConfig.h"
            "FreeRTOS-Kernel-SMP/portable/xtensa/include/freertos") # Xtensa headers via #include "xxx.h"

        set(private_include_dirs
            "FreeRTOS-Kernel-SMP/portable/xtensa/include/freertos"
            "FreeRTOS-Kernel-SMP/portable/xtensa"
            .)

    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        set(srcs
            "FreeRTOS-Kernel-SMP/portable/riscv/port.c"
            "FreeRTOS-Kernel-SMP/portable/riscv/portasm.S")

        set(include_dirs
            FreeRTOS-Kernel-SMP/include
            esp_additions/include/freertos          # For files with #include "FreeRTOSConfig.h"
            FreeRTOS-Kernel-SMP/portable/riscv/include
            esp_additions/include                   # For files with #include "freertos/FreeRTOSConfig.h"
            FreeRTOS-Kernel-SMP/portable/riscv/include/freertos)

        set(private_include_dirs
            FreeRTOS-Kernel-SMP/portable/riscv/include/freertos
            FreeRTOS-Kernel-SMP/portable/riscv
            .)

    endif()

    list(APPEND srcs
        "FreeRTOS-Kernel-SMP/croutine.c"
        "FreeRTOS-Kernel-SMP/event_groups.c"
        "FreeRTOS-Kernel-SMP/list.c"
        "FreeRTOS-Kernel-SMP/queue.c"
        "FreeRTOS-Kernel-SMP/tasks.c"
        "FreeRTOS-Kernel-SMP/timers.c"
        "FreeRTOS-Kernel-SMP/stream_buffer.c"
        "FreeRTOS-openocd.c"
        )

    list(APPEND private_include_dirs
        "FreeRTOS-Kernel-SMP/include/freertos"   # FreeRTOS headers via #include "xxx.h"
        "esp_additions/private_include")    # For include "freertos_tasks_c_additions.h"

    if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
        list(APPEND srcs "FreeRTOS-Kernel-SMP/portable/xtensa/xtensa_loadstore_handler.S")
    endif()

else()
    set(ldfragments linker.lf)
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
            esp_additions/include/freertos          # For files with #include "FreeRTOSConfig.h"
            FreeRTOS-Kernel/portable/xtensa/include # For arch-specific FreeRTOSConfig_arch.h in portable/<arch>/include
            esp_additions/include)                  # For files with #include "freertos/FreeRTOSConfig.h"

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
            esp_additions/include/freertos          # For files with #include "FreeRTOSConfig.h"
            FreeRTOS-Kernel/portable/riscv/include  # For arch-specific FreeRTOSConfig_arch.h in portable/<arch>/include
            esp_additions/include)                  # For files with #include "freertos/FreeRTOSConfig.h"

        set(private_include_dirs
            FreeRTOS-Kernel/portable/riscv/include/freertos
            FreeRTOS-Kernel/portable/riscv
            FreeRTOS-Kernel/portable/priv_include
            .)

    endif()

    list(APPEND srcs
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
        "FreeRTOS-Kernel/include/freertos"
        "esp_additions/private_include")    # For include "freertos_tasks_c_additions.h"

    if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
        list(APPEND srcs "FreeRTOS-Kernel/portable/xtensa/xtensa_loadstore_handler.S")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS  ${private_include_dirs}
                    LDFRAGMENTS "${ldfragments}"
                    PRIV_REQUIRES soc esp_pm)

idf_component_get_property(COMPONENT_DIR freertos COMPONENT_DIR)

if(CONFIG_FREERTOS_SMP)
    idf_component_set_property(freertos ORIG_INCLUDE_PATH "${COMPONENT_DIR}/FreeRTOS-Kernel-SMP/include/freertos/")
else()
    idf_component_set_property(freertos ORIG_INCLUDE_PATH "${COMPONENT_DIR}/FreeRTOS-Kernel/include/freertos/")
endif()

if(CONFIG_FREERTOS_DEBUG_OCDAWARE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=uxTopUsedPriority") #will be removed
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=FreeRTOS_openocd_params")
    idf_build_set_property(COMPILE_OPTIONS "-DconfigENABLE_FREERTOS_DEBUG_OCDAWARE=1" APPEND)
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

if(CONFIG_APPTRACE_SV_ENABLE)
    # FreeRTOS headers have a dependency on app_trace when SystemView tracing is enabled
    idf_component_optional_requires(PUBLIC app_trace)
elseif(CONFIG_APPTRACE_ENABLE)
    # [refactor-todo]: port.c has a dependency on esp_apptrace_init, for running it on APP CPU.
    # this should be resolved when link-time registration of startup functions is added.
    idf_component_optional_requires(PRIVATE app_trace)
endif()

if(CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME)
    # [refactor-todo]: port.c esp_startup_start_app_common() calls esp_gdbstub_init()
    idf_component_optional_requires(PRIVATE esp_gdbstub)
endif()

if(CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER)
    # [refactor-todo]: esp_timer is required by FreeRTOS when we use esp_timer_get_time() to do profiling
    # Introduce a port wrapper function to avoid including esp_timer.h into the public header
    idf_component_optional_requires(PUBLIC esp_timer)
endif()

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()
