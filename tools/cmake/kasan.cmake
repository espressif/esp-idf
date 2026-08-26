# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Kernel Address Sanitizer (CONFIG_COMPILER_KASAN) exclusion policy.
#
# Included by both build systems so that the set of components excluded from
# instrumentation has a single definition. The only build system API relied on
# here is idf_component_get_property(), which both provide with the same
# signature. Applying the result is left to each build system, because the
# component-name to library-target mapping differs between them.

# Store in <var> the subset of the given component names that must not be
# compiled with KASAN instrumentation. A component qualifies either because it
# is part of the built-in low-level set below, or because it declares the
# NO_KASAN component property in its own CMakeLists.txt.
#
# Rationale per bucket:
#   - hal / soc / esp_rom + every esp_hal_* peripheral HAL: perform volatile
#     MMIO accesses (outside the shadow window, so each check is a no-op but
#     still pays the indirect call into __asan_load* / __asan_store*).
#   - spi_flash: runs with the flash cache disabled.
#   - esp_hw_support: RTC/PMU register access, runs with MSPI bus held.
#   - bootloader_support: runs before kasan_init_shadow().
#   - freertos: scheduler / ISR plumbing is too hot to instrument.
#   - heap: walks its own TLSF metadata living inside the (poisoned) pool;
#     instrumented metadata walks would self-trigger. Shadow updates are
#     handled explicitly via heap_kasan.c / heap_kasan_hooks.c, which are
#     individually compiled with -fno-sanitize via set_source_files_properties.
function(kasan_filter_excluded_components var)
    set(excluded_components
        hal soc esp_rom
        spi_flash
        esp_hw_support
        bootloader_support
        freertos
        heap)

    # Match every esp_hal_* peripheral HAL component dynamically instead of
    # listing them one by one, so newly added HAL components stay excluded
    # without revisiting this file.
    set(excluded_pattern "^esp_hal_")

    set(excluded "")
    foreach(component_name IN LISTS ARGN)
        if(component_name IN_LIST excluded_components
                OR component_name MATCHES "${excluded_pattern}")
            list(APPEND excluded "${component_name}")
            continue()
        endif()

        # Any other component, including one shipped by a user or pulled from
        # the component registry, opts out by setting the NO_KASAN component
        # property in its own CMakeLists.txt.
        idf_component_get_property(no_kasan "${component_name}" NO_KASAN)
        if(no_kasan)
            list(APPEND excluded "${component_name}")
        endif()
    endforeach()

    set(${var} "${excluded}" PARENT_SCOPE)
endfunction()
