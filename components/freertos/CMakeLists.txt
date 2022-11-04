if(BOOTLOADER_BUILD)
    # bootloader only needs FreeRTOS for config, not for anything else
    idf_component_register()
    return()
endif()

idf_build_get_property(target IDF_TARGET)

if(CONFIG_FREERTOS_SMP)
    set(kernel_dir "FreeRTOS-Kernel-SMP")
else()
    set(kernel_dir "FreeRTOS-Kernel")
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
set(arch "xtensa")
elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
set(arch "riscv")
elseif(${target} STREQUAL "linux")
set(arch "linux")
endif()

set(srcs
    "${kernel_dir}/list.c"
    "${kernel_dir}/queue.c"
    "${kernel_dir}/tasks.c"
    "${kernel_dir}/timers.c"
    "${kernel_dir}/croutine.c"
    "${kernel_dir}/event_groups.c"
    "${kernel_dir}/stream_buffer.c"
    "${kernel_dir}/portable/${arch}/port.c")

set(include_dirs
    "${kernel_dir}/include" # FreeRTOS headers via #include "freertos/xxx.h"
    "${kernel_dir}/portable/${arch}/include" # For arch-specific FreeRTOSConfig_arch.h in portable/<arch>/include
    "esp_additions/include/freertos"         # For files with #include "FreeRTOSConfig.h"
    "esp_additions/include")                 # For files with #include "freertos/FreeRTOSConfig.h"
                                             #      or #include "freertos/task_snapshot.h"

set(private_include_dirs
    "${kernel_dir}/portable/${arch}/include/freertos"
    "${kernel_dir}/portable/${arch}"
    "${kernel_dir}/include/freertos"   # FreeRTOS headers via #include "xxx.h"
    .)

set(private_requirements "")

if(${target} STREQUAL "linux")
    list(APPEND srcs
        "${kernel_dir}/portable/${arch}/port_idf.c"
        "${kernel_dir}/portable/${arch}/utils/wait_for_event.c")
else()
    list(APPEND srcs
        "FreeRTOS-openocd.c"
        "${kernel_dir}/portable/${arch}/portasm.S")

    list(APPEND private_include_dirs
        "esp_additions/private_include")    # For include "freertos_tasks_c_additions.h"

    if(CONFIG_FREERTOS_SMP)
        set(ldfragments linker_smp.lf)
        list(APPEND include_dirs "${kernel_dir}/portable/${arch}/include/freertos") # Xtensa headers via #include "xx.h"
    else()
        list(APPEND srcs
            "${kernel_dir}/portable/port_common.c"
            "${kernel_dir}/portable/port_systick.c"
            "esp_additions/freertos_v8_compat.c")

        list(APPEND private_include_dirs "${kernel_dir}/portable/priv_include") # For port_systick.h on normal FreeRTOS

        set(ldfragments linker.lf)
    endif()

    list(APPEND private_requirements soc esp_pm)
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND srcs
        "${kernel_dir}/portable/${arch}/xtensa_context.S"
        "${kernel_dir}/portable/${arch}/xtensa_init.c"
        "${kernel_dir}/portable/${arch}/xtensa_overlay_os_hook.c"
        "${kernel_dir}/portable/${arch}/xtensa_vector_defaults.S"
        "${kernel_dir}/portable/${arch}/xtensa_vectors.S")
endif()


if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    list(APPEND srcs "${kernel_dir}/portable/xtensa/xtensa_loadstore_handler.S")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS  ${private_include_dirs}
                    LDFRAGMENTS "${ldfragments}"
                    PRIV_REQUIRES "${private_requirements}")

if(${target} STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "projCOVERAGE_TEST=0")
    target_link_libraries(${COMPONENT_LIB} PUBLIC pthread)
else()
    idf_component_get_property(COMPONENT_DIR freertos COMPONENT_DIR)

    idf_component_set_property(freertos ORIG_INCLUDE_PATH "${COMPONENT_DIR}/${kernel_dir}/include/freertos/")

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

    if(CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP AND CONFIG_FREERTOS_SMP)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=vPortCleanUpTCB")
    endif()
endif()
