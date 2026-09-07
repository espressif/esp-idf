# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

# ESP-IDF LLVM optimization framework: map menuconfig-selected optimizations to
# compiler flags. Component CMakeLists only mark opt-in scopes (file / list /
# component). Flags for each menuconfig option are also published as
# IDF_LLVM_OPT_* for standard CMake (third-party).

function(__idf_llvm_opt_collect_flags memcpy_out all_out)
    set(memcpy "")
    set(all "")

    if(CONFIG_IDF_TOOLCHAIN_CLANG AND CMAKE_C_COMPILER_ID MATCHES "Clang")
        if(CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION)
            list(APPEND memcpy "-mllvm=-riscv-esp32-p4-mem-intrin")
            # PIE 2.1 vs 2.2 comes from -mcpu (components/soc/project_include.cmake).
            # +espv-lowering is still required for .m intrinsic isel (vld/vst.128.ip.m).
            # Do not pass +xespv2p1: that name is not a Clang target-feature and is ignored.
            list(APPEND memcpy "-Xclang" "-target-feature" "-Xclang" "+espv-lowering")
        endif()
        list(APPEND all ${memcpy})
    endif()

    set(${memcpy_out} "${memcpy}" PARENT_SCOPE)
    set(${all_out} "${all}" PARENT_SCOPE)
endfunction()

# SHELL-wrapped lists for target_compile_options. Empty when Kconfig is off or not Clang.
function(__idf_llvm_opt_publish_flags)
    __idf_llvm_opt_collect_flags(memcpy_flags all_flags)
    __idf_llvm_opt_shell_wrap_xclang("${memcpy_flags}" memcpy_wrapped)
    __idf_llvm_opt_shell_wrap_xclang("${all_flags}" all_wrapped)
    set(IDF_LLVM_OPT_MEMCPY "${memcpy_wrapped}" CACHE INTERNAL
        "Flags for CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION; empty if off or not Clang" FORCE)
    set(IDF_LLVM_OPT_ALL "${all_wrapped}" CACHE INTERNAL
        "Flags for all LLVM optimizations enabled in menuconfig; empty if none or not Clang" FORCE)
endfunction()

function(__idf_llvm_opt_get_options output)
    set(options ${ARGN})
    __idf_llvm_opt_publish_flags()

    if(NOT CONFIG_IDF_TOOLCHAIN_CLANG OR NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(${output} "" PARENT_SCOPE)
        return()
    endif()

    __idf_llvm_opt_collect_flags(_memcpy all_flags)
    list(APPEND options ${all_flags})
    set(${output} "${options}" PARENT_SCOPE)
endfunction()

# target_compile_options de-duplicates identical tokens, which splits
# "-Xclang" "<arg>" pairs. SHELL: keeps each consecutive -Xclang group intact.
# Source COMPILE_OPTIONS are not de-duplicated and do not honour SHELL:.
function(__idf_llvm_opt_shell_wrap_xclang input output)
    set(result "")
    set(i 0)
    list(LENGTH input n)
    while(i LESS n)
        list(GET input ${i} item)
        if(item STREQUAL "-Xclang")
            set(group "")
            while(i LESS n)
                list(GET input ${i} tok)
                if(NOT tok STREQUAL "-Xclang")
                    break()
                endif()
                math(EXPR nxt "${i} + 1")
                if(nxt GREATER_EQUAL n)
                    message(FATAL_ERROR "LLVM opt flags: -Xclang missing argument")
                endif()
                list(GET input ${nxt} arg)
                if(group STREQUAL "")
                    set(group "-Xclang ${arg}")
                else()
                    string(APPEND group " -Xclang ${arg}")
                endif()
                math(EXPR i "${nxt} + 1")
            endwhile()
            list(APPEND result "SHELL:${group}")
        else()
            list(APPEND result "${item}")
            math(EXPR i "${i} + 1")
        endif()
    endwhile()
    set(${output} "${result}" PARENT_SCOPE)
endfunction()

function(__idf_apply_llvm_opt_to_target target)
    __idf_llvm_opt_get_options(options ${ARGN})
    if(options)
        __idf_llvm_opt_shell_wrap_xclang("${options}" options)
        target_compile_options(${target} PRIVATE ${options})
    endif()
endfunction()

function(__idf_apply_llvm_opt_to_sources)
    set(multi_value SRCS OPTIONS)
    cmake_parse_arguments(ARG "" "" "${multi_value}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown idf_component_enable_llvm_opt arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    __idf_llvm_opt_get_options(options ${ARG_OPTIONS})
    if(options)
        set_property(SOURCE ${ARG_SRCS} APPEND PROPERTY COMPILE_OPTIONS ${options})
    endif()
endfunction()

# Apply menuconfig-selected LLVM optimizations to one source, a source list, or
# the whole current component. OPTIONS is an advanced escape hatch for custom
# Clang/LLVM optimization options and is ignored with non-Clang toolchains.
function(idf_component_enable_llvm_opt)
    set(multi_value SRCS OPTIONS)
    cmake_parse_arguments(ARG "" "" "${multi_value}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown idf_component_enable_llvm_opt arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if(ARG_SRCS)
        __idf_apply_llvm_opt_to_sources(SRCS ${ARG_SRCS} OPTIONS ${ARG_OPTIONS})
        return()
    endif()

    if(COMPONENT_LIB AND TARGET ${COMPONENT_LIB})
        set(component_target ${COMPONENT_LIB})
    elseif(COMPONENT_TARGET AND TARGET ${COMPONENT_TARGET})
        set(component_target ${COMPONENT_TARGET})
    else()
        message(FATAL_ERROR "idf_component_enable_llvm_opt must be called after idf_component_register")
    endif()

    __idf_apply_llvm_opt_to_target(${component_target} ${ARG_OPTIONS})
endfunction()
