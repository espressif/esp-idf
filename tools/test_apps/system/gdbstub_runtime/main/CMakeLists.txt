set(srcs "test_app_main.c")
if(CONFIG_IDF_TARGET_ARCH_RISCV AND CONFIG_SOC_CPU_HAS_HWLOOP)
    list(APPEND srcs "xesppie_loops.S")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ""
                       REQUIRES esp_gdbstub)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-unused-label")
