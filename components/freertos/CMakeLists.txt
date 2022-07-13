if(BOOTLOADER_BUILD)
    # bootloader only needs FreeRTOS for config, not for anything else
    idf_component_register()
    return()
endif()

idf_build_get_property(target IDF_TARGET)

if(NOT "${target}" STREQUAL "esp32c3")  # should test arch here not target, TODO ESP32-C3 IDF-1754
    set(srcs
        "port/xtensa/port.c"
        "port/xtensa/portasm.S"
        "port/xtensa/xtensa_context.S"
        "port/xtensa/xtensa_init.c"
        "port/xtensa/xtensa_overlay_os_hook.c"
        "port/xtensa/xtensa_vector_defaults.S"
        "port/xtensa/xtensa_vectors.S")

    set(include_dirs
        include
        include/esp_additions
        port/xtensa/include)

    set(private_include_dirs
        include/freertos
        port/xtensa/include/freertos
        port/xtensa
        .)

    set(required_components app_trace esp_timer)
else()  # RISC-V
    set(srcs
        "port/riscv/port.c"
        "port/riscv/portasm.S")

    set(include_dirs
        include
        include/esp_additions
        port/riscv/include)

    set(private_include_dirs
        include/freertos
        port/riscv/include/freertos
        port/riscv
        .)

    set(required_components app_trace esp_timer)
endif()

list(APPEND srcs
    "port/port_common.c"
    "croutine.c"
    "event_groups.c"
    "list.c"
    "queue.c"
    "tasks.c"
    "timers.c"
    "stream_buffer.c"
    "FreeRTOS-openocd.c"
    "freertos_v8_compat.c")

list(APPEND private_include_dirs "esp_additions/private_include")    # For include "freertos_tasks_c_additions.h"

if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    list(APPEND srcs "port/xtensa/xtensa_loadstore_handler.S")
endif()

# app_trace is required by FreeRTOS headers only when CONFIG_APPTRACE_SV_ENABLE=y,
# but requirements can't depend on config options, so always require it.
idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS  ${private_include_dirs}
                    LDFRAGMENTS linker.lf
                    REQUIRES ${required_components}
                    PRIV_REQUIRES soc)

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
