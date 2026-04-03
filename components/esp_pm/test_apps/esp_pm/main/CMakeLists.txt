set(sources "test_app_main.c"
            "test_pm.c")

if(CONFIG_SOC_CPU_HAS_FPU AND CONFIG_IDF_TARGET_ARCH_RISCV AND CONFIG_SOC_PM_FPU_RETENTION_BY_SW)
    list(APPEND sources "test_fpu_retention.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component must be registered as a WHOLE_ARCHIVE
idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_pm ulp esp_timer esp_psram esp_driver_gptimer vfs esp_hal_gpio
                       WHOLE_ARCHIVE)
