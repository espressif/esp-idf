# FreeRTOS Component Architecture
#
# The FreeRTOS component mainly contains
# - Different FreeRTOS kernel implementations (namely IDF FreeRTOS and Amazon SMP FreeRTOS).
# - Different ports of each architecture for each kernel implementation.
# - IDF additions to FreeRTOS (e.g., features and API) to augment FreeRTOS
#
# The FreeRTOS component organizes its files as follows
#
# - `./config`
#       - Contains all "FreeRTOSConfig.h" files required by FreeRTOS
# - `./esp_additions`
#       - Additional features added by ESP-IDF to augment FreeRTOS, and not part of the original kernel
# - `./FreeRTOS-Kernel-...`
#       - Different FreeRTOS kernel implementations. Each implementation is kept in its own directory.
#       - Ports for the implementation are kept in `FreeRTOS-Kernel-.../portable/xxx/`\
# - `./test_apps`
#       - Contains all unit tests/test apps for the FreeRTOS component.
# - `./`
#       - Files common across all kernel implementations and all ports

# Bootloader builds only needs FreeRTOS for config, not for anything else
if(BOOTLOADER_BUILD)
    idf_component_register()
    return()
endif()

# Set some convenience variables
idf_build_get_property(target IDF_TARGET)

if(CONFIG_FREERTOS_SMP)
    set(kernel_impl "FreeRTOS-Kernel-SMP")
else()
    set(kernel_impl "FreeRTOS-Kernel")
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    set(arch "xtensa")
elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    set(arch "riscv")
elseif(${target} STREQUAL "linux")
    set(arch "linux")
endif()

set(srcs "")
set(include_dirs "")
set(private_include_dirs "")
set(private_requirements "")
set(ldfragments "")

# ---------------------------------------------------- Set Sources -----------------------------------------------------

# Add common source files
list(APPEND srcs
    "heap_idf.c")

if((arch STREQUAL "xtensa") OR (arch STREQUAL "riscv"))
    list(APPEND srcs
        "app_startup.c"
        "port_common.c"
        "port_systick.c")
endif()

# Add FreeRTOS Kernel source files
list(APPEND srcs
    "${kernel_impl}/list.c"
    "${kernel_impl}/queue.c"
    "${kernel_impl}/tasks.c"
    "${kernel_impl}/timers.c"
    "${kernel_impl}/event_groups.c"
    "${kernel_impl}/stream_buffer.c")

# Add port source files
list(APPEND srcs
    "${kernel_impl}/portable/${arch}/port.c")

if(arch STREQUAL "linux")
    list(APPEND srcs
        "${kernel_impl}/portable/${arch}/utils/wait_for_event.c")
    if(kernel_impl STREQUAL "FreeRTOS-Kernel")
        list(APPEND srcs
            "${kernel_impl}/portable/${arch}/port_idf.c")
    endif()
else()
    list(APPEND srcs
        "${kernel_impl}/portable/${arch}/portasm.S")
endif()

if(arch STREQUAL "xtensa")
    list(APPEND srcs
        "${kernel_impl}/portable/${arch}/xtensa_init.c"
        "${kernel_impl}/portable/${arch}/xtensa_overlay_os_hook.c")
endif()

# Add ESP-additions source files
list(APPEND srcs
    "esp_additions/freertos_compatibility.c"
    "esp_additions/idf_additions.c")

if(arch STREQUAL "linux")
    # Check if we need to address the FreeRTOS EINTR coexistence with linux system calls if we're building without
    # lwIP enabled, we need to use linux system select which will receive EINTR event on every FreeRTOS interrupt, we
    # workaround this problem by wrapping select() to bypass and silence the EINTR events
    set(BYPASS_EINTR_ISSUE 0)
    if(NOT CONFIG_LWIP_ENABLE)
        set(BYPASS_EINTR_ISSUE 1)
        list(APPEND srcs "esp_additions/FreeRTOSSimulator_wrappers.c")
    endif()
endif()

# ------------------------------------------------ Set Public Includes -------------------------------------------------

# Add common public include directories
list(APPEND include_dirs
    "config/include"                # For `#include "freertos/FreeRTOSConfig.h"`
    "config/include/freertos"       # For `#include "FreeRTOSConfig.h"`
    "config/${arch}/include")       # For `#include "freertos/FreeRTOSConfig_arch.h"`

# Add FreeRTOS Kernel public include directories
list(APPEND include_dirs
    "${kernel_impl}/include")       # FreeRTOS headers via `#include "freertos/xxx.h"`

# Add port public include directories
list(APPEND include_dirs
    "${kernel_impl}/portable/${arch}/include"               # For port headers via `#include "freertos/...h"`
    "${kernel_impl}/portable/${arch}/include/freertos")     # For port headers via `#include "...h"`

# Add ESP-additions public include directories
list(APPEND include_dirs
    "esp_additions/include")        # For ESP-addition headers via
                                    #   - `#include "freertos/...h"`
                                    #   - `#include "esp_private/...h"`

# ----------------------------------------------- Set Private Includes -------------------------------------------------

# Add common private include directories
if((arch STREQUAL "xtensa") OR (arch STREQUAL "riscv"))
    list(APPEND private_include_dirs
        ".")                        # For `#include "port_systick.h"
endif()

# Add FreeRTOS Kernel private include directories
list(APPEND private_include_dirs
    "${kernel_impl}/include/freertos")      # FreeRTOS headers via `#include "xxx.h"`

# Add port private include directories
if(arch STREQUAL "linux")
    list(APPEND private_include_dirs
        "${kernel_impl}/portable/${arch}/") # Linux port `#include "utils/wait_for_event.h"`
endif()

# Add ESP-additions private include directories
list(APPEND private_include_dirs
    "esp_additions")                # For `include "freertos_tasks_c_additions.h"`

# ------------------------------------------------------- Misc ---------------------------------------------------------

# Add linker fragments
list(APPEND ldfragments
    "linker_common.lf")

if((arch STREQUAL "xtensa") OR (arch STREQUAL "riscv"))
    if(kernel_impl STREQUAL "FreeRTOS-Kernel-SMP")
        list(APPEND ldfragments
            "linker_smp.lf")
    else()
        list(APPEND ldfragments
            "linker.lf")
    endif()
endif()

# ------------------------------------------------ Register Component --------------------------------------------------

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS ${private_include_dirs}
                    LDFRAGMENTS ${ldfragments}
                    PRIV_REQUIRES ${private_requirements})

if(arch STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "projCOVERAGE_TEST=0")
    target_link_libraries(${COMPONENT_LIB} PUBLIC pthread)
    if(BYPASS_EINTR_ISSUE)
        target_link_libraries(${COMPONENT_LIB} PRIVATE dl)
    endif()

    # Disable strict prototype warnings in upstream code
    # (struct event * event_create() is missing 'void')
    set_source_files_properties(
        "${kernel_impl}/portable/${arch}/utils/wait_for_event.c"
        PROPERTIES COMPILE_OPTIONS
        "-Wno-strict-prototypes"
        )
else()
    idf_component_get_property(COMPONENT_DIR freertos COMPONENT_DIR)

    idf_component_set_property(freertos ORIG_INCLUDE_PATH "${COMPONENT_DIR}/${kernel_impl}/include/freertos/")

    if(CONFIG_FREERTOS_DEBUG_OCDAWARE)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=FreeRTOS_openocd_params")
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
        # [refactor-todo]: app_startup.c esp_startup_start_app_other_cores() has a dependency on esp_apptrace_init()
        # (called on CPU1). This should be resolved when link-time registration of startup functions is added.
        idf_component_optional_requires(PRIVATE app_trace)
    endif()

    if(CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME)
        # [refactor-todo]: app_startup.c esp_startup_start_app_other_cores() calls esp_gdbstub_init() (called on CPU0).
        # This should be resolved when link-time registration of startup functions is added.
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

    if(CONFIG_PM_TRACE)
        # esp_pm is required by port_systick.c for tracing
        idf_component_optional_requires(PRIVATE esp_pm)
    endif()

endif()
