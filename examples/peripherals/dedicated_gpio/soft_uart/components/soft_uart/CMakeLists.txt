set(srcs "soft_uart.c")

# During CMake early expansion, Kconfig constants are not defined yet, thus, to
# avoid having CMake falsely fail on all targets, do not send FATAL_ERROR at that moment
if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    if(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND srcs "riscv/soft_uart.S")
    else()
        list(APPEND srcs "xtensa/soft_uart.S")
    endif()
elseif(NOT CMAKE_BUILD_EARLY_EXPANSION)
    message(FATAL_ERROR "Target doesn't support dedicated gpios")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver
                       LDFRAGMENTS linker.lf)
