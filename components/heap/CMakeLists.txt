idf_build_get_property(target IDF_TARGET)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "heap_caps_linux.c"
                           INCLUDE_DIRS "include")
    return()
endif()

set(srcs "heap_caps_base.c"
         "heap_caps.c"
         "heap_caps_init.c"
         "multi_heap.c")

# the root dir of TLSF submodule contains headers with static inline
# functions used in the heap ROM patches. Therefore,
# the tlsf/ dir must be included in the list of public includes.
set(includes "include"
             "tlsf")

# The TLSF "public" includes should only be used
# inside the heap component and are therefore added
# to the list of the private includes from the heap
# component perspective
set(priv_includes "private_include"
                  "tlsf/include")

if(NOT CONFIG_HEAP_TLSF_USE_ROM_IMPL)
    list(APPEND srcs "tlsf/tlsf.c")
endif()

if(NOT CONFIG_HEAP_POISONING_DISABLED)
    list(APPEND srcs "multi_heap_poisoning.c")
endif()

if(CONFIG_HEAP_TASK_TRACKING)
    list(APPEND srcs "heap_task_info.c")
endif()

if(CONFIG_HEAP_TRACING_STANDALONE)
    list(APPEND srcs "heap_trace_standalone.c")
    set_source_files_properties(heap_trace_standalone.c
        PROPERTIES COMPILE_FLAGS
        -Wno-frame-address)
endif()

# Add SoC memory layout to the sources

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "port/memory_layout_utils.c")
    list(APPEND srcs "port/${target}/memory_layout.c")

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL)
        list(APPEND includes "rom_patches/include"
                             "rom_patches/${target}")

        if(CONFIG_ESP_ROM_TLSF_CHECK_PATCH)
            # This file shall be included in the build if TLSF in ROM is activated
            list(APPEND srcs "rom_patches/rom_patch_tlsf.c")
        endif()

        if(CONFIG_ESP_ROM_MULTI_HEAP_WALK_PATCH)
            # This file shall be included in the build if TLSF in ROM is activated
            list(APPEND srcs "rom_patches/rom_patch_multi_heap.c")
        endif()
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_INCLUDE_DIRS ${priv_includes}
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES soc)

if(CONFIG_HEAP_TLSF_USE_ROM_IMPL AND NOT BOOTLOADER_BUILD)
    # After registering the component, set the tlsf_set_rom_patches symbol as undefined
    # to force the linker to integrate the whole rom_patch_tlsf.c object file inside the
    # final binary. This is necessary because tlsf_set_rom_patches is a constructor, thus,
    # there is no explicit reference/call to it in IDF.
    if(CONFIG_ESP_ROM_TLSF_CHECK_PATCH)
        target_link_libraries(${COMPONENT_LIB} PRIVATE "-u tlsf_set_rom_patches")
    endif()

    if(CONFIG_ESP_ROM_MULTI_HEAP_WALK_PATCH)
        target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_rom_include_multi_heap_patch")
    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "port/${target}/ld/${target}.rom.heap.ld")
endif()

if(CONFIG_HEAP_TRACING)
    set(WRAP_FUNCTIONS
        heap_caps_realloc_base
        heap_caps_malloc_base
        heap_caps_aligned_alloc_base
        heap_caps_free)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()
endif()

if(IDF_BUILD_V2)
    target_compile_options(${COMPONENT_TARGET} PRIVATE "$<$<TARGET_EXISTS:idf::freertos>:-DMULTI_HEAP_FREERTOS>")
else()
    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        idf_build_get_property(build_components BUILD_COMPONENTS)
        if(freertos IN_LIST build_components)
            target_compile_options(${COMPONENT_TARGET} PRIVATE "-DMULTI_HEAP_FREERTOS")
        endif()
    endif()
endif()
