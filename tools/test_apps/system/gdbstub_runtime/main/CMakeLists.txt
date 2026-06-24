set(srcs "test_app_main.c")
if(CONFIG_SOC_CPU_HAS_HWLOOP)
    list(APPEND srcs "xesppie_loops.S")
endif()
if(CONFIG_SOC_CPU_HAS_FPU OR CONFIG_SOC_CPU_HAS_PIE)
    list(APPEND srcs "coproc_regs.c")
endif()
if(CONFIG_IDF_TARGET_ARCH_RISCV)
    set(ext_comp "riscv")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ""
                       REQUIRES esp_gdbstub ${ext_comp})

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-unused-label")
