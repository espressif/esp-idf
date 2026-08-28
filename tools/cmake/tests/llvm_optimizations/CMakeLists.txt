# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22)
# Never compiles: only inspects COMPILE_OPTIONS. Host CI (Windows minimal
# cmake jobs) has no C compiler on PATH, so skip compiler detection.
set(CMAKE_C_COMPILER "${CMAKE_COMMAND}" CACHE FILEPATH "" FORCE)
set(CMAKE_C_COMPILER_WORKS TRUE CACHE BOOL "" FORCE)
set(CMAKE_C_COMPILER_FORCED TRUE)
project(llvm_optimizations_test C)

include(${CMAKE_CURRENT_LIST_DIR}/../../llvm_optimizations.cmake)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/component.c "void component(void) {}\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/selected.c "void selected(void) {}\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/plain.c "void plain(void) {}\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gcc.c "void gcc_source(void) {}\n")

set(CMAKE_C_COMPILER_ID Clang)
set(CONFIG_IDF_TOOLCHAIN_CLANG ON)
set(CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION ON)

add_library(component STATIC ${CMAKE_CURRENT_BINARY_DIR}/component.c)
set(COMPONENT_LIB component)
idf_component_enable_llvm_opt()
get_target_property(component_options component COMPILE_OPTIONS)
list(JOIN component_options " " component_options)
if(NOT component_options MATCHES "riscv-esp32-p4-mem-intrin")
    message(FATAL_ERROR "Component scope is missing the selected LLVM optimization")
endif()
if(NOT component_options MATCHES "SHELL:")
    message(FATAL_ERROR "Component scope must SHELL-wrap -Xclang pairs")
endif()
if(component_options MATCHES "xespv2p1")
    message(FATAL_ERROR "PIE revision must come from -mcpu, not +xespv2p1")
endif()
if(NOT component_options MATCHES "espv-lowering")
    message(FATAL_ERROR "Component scope is missing +espv-lowering for .m intrinsic isel")
endif()

if(NOT IDF_LLVM_OPT_ALL OR NOT IDF_LLVM_OPT_MEMCPY)
    message(FATAL_ERROR "Exported IDF_LLVM_OPT_ALL / IDF_LLVM_OPT_MEMCPY must be set when memcpy is enabled")
endif()
if(NOT "${IDF_LLVM_OPT_ALL}" STREQUAL "${IDF_LLVM_OPT_MEMCPY}")
    message(FATAL_ERROR "With only the memcpy optimization enabled, IDF_LLVM_OPT_ALL must equal IDF_LLVM_OPT_MEMCPY")
endif()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/third_party.c "void third_party(void) {}\n")
add_library(fake_lvgl STATIC ${CMAKE_CURRENT_BINARY_DIR}/third_party.c)
target_compile_options(fake_lvgl PRIVATE ${IDF_LLVM_OPT_ALL})
get_target_property(third_party_options fake_lvgl COMPILE_OPTIONS)
list(JOIN third_party_options " " third_party_options)
if(NOT third_party_options MATCHES "riscv-esp32-p4-mem-intrin")
    message(FATAL_ERROR "Standard CMake apply of IDF_LLVM_OPT_ALL missed the memcpy flags")
endif()
if(NOT third_party_options MATCHES "SHELL:")
    message(FATAL_ERROR "IDF_LLVM_OPT_ALL must be SHELL-wrapped for target_compile_options")
endif()

add_library(selected STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/selected.c
    ${CMAKE_CURRENT_BINARY_DIR}/plain.c)
set(COMPONENT_LIB selected)
idf_component_enable_llvm_opt(
    SRCS ${CMAKE_CURRENT_BINARY_DIR}/selected.c
    OPTIONS "-custom-llvm-option")
get_source_file_property(selected_options ${CMAKE_CURRENT_BINARY_DIR}/selected.c COMPILE_OPTIONS)
get_source_file_property(plain_options ${CMAKE_CURRENT_BINARY_DIR}/plain.c COMPILE_OPTIONS)
list(JOIN selected_options " " selected_options)
if(NOT selected_options MATCHES "riscv-esp32-p4-mem-intrin"
   OR NOT selected_options MATCHES "custom-llvm-option")
    message(FATAL_ERROR "Source scope is missing selected or custom options")
endif()
if(selected_options MATCHES "SHELL:")
    message(FATAL_ERROR "Source COMPILE_OPTIONS must not use SHELL:")
endif()
if(plain_options)
    message(FATAL_ERROR "LLVM optimization leaked to an unselected source")
endif()

set(CMAKE_C_COMPILER_ID GNU)
unset(CONFIG_IDF_TOOLCHAIN_CLANG)
add_library(gcc_component STATIC ${CMAKE_CURRENT_BINARY_DIR}/gcc.c)
set(COMPONENT_LIB gcc_component)
idf_component_enable_llvm_opt(OPTIONS "-custom-llvm-option")
get_target_property(gcc_options gcc_component COMPILE_OPTIONS)
if(gcc_options)
    message(FATAL_ERROR "LLVM optimization options must be ignored for non-Clang compilers")
endif()
if(IDF_LLVM_OPT_ALL OR IDF_LLVM_OPT_MEMCPY)
    message(FATAL_ERROR "Exported LLVM opt flags must be empty for non-Clang compilers")
endif()
