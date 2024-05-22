set(INCLUDE_FILES "include" . include PUBLIC soc)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND INCLUDE_FILES "include/riscv")
endif()

target_include_directories(${COMPONENT_LIB} PRIVATE ${INCLUDE_FILES} include/private)

set(srcs "cpu_start.c" "panic_handler.c" "esp_system_chip.c")

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    list(APPEND srcs "image_process.c")
endif()

if(CONFIG_SOC_BOD_SUPPORTED)
    list(APPEND srcs "brownout.c")
endif()

if(CONFIG_ESP_CONSOLE_USB_CDC)
    list(APPEND srcs "usb_console.c")
endif()

if(CONFIG_ESP_IPC_ISR_ENABLE)
    list(APPEND srcs "esp_ipc_isr.c")
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        list(APPEND srcs "arch/xtensa/esp_ipc_isr_port.c"
                         "arch/xtensa/esp_ipc_isr_handler.S"
                         "arch/xtensa/esp_ipc_isr_routines.S")
    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND srcs "arch/riscv/esp_ipc_isr_port.c"
                         "arch/riscv/esp_ipc_isr_handler.S"
                         "arch/riscv/esp_ipc_isr_routines.c")
    endif()
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND srcs "arch/xtensa/panic_arch.c"
                     "arch/xtensa/panic_handler_asm.S"
                     "arch/xtensa/expression_with_stack.c"
                     "arch/xtensa/expression_with_stack_asm.S"
                     "arch/xtensa/debug_helpers.c"
                     "arch/xtensa/debug_helpers_asm.S"
                     "arch/xtensa/debug_stubs.c"
                     "arch/xtensa/trax.c")
elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND srcs "arch/riscv/expression_with_stack.c"
                     "arch/riscv/panic_arch.c"
                     "arch/riscv/debug_helpers.c"
                     "arch/riscv/debug_stubs.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

idf_build_get_property(target IDF_TARGET)
add_subdirectory(soc/${target})
