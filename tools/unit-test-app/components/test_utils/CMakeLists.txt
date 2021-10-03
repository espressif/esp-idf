set(srcs "ccomp_timer.c"
         "test_runner.c"
         "test_utils.c")

if(CONFIG_IDF_TARGET_ESP32)
    list(APPEND srcs "ref_clock_impl_rmt_pcnt.c")
else()
    list(APPEND srcs "ref_clock_impl_timergroup.c")
endif()

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND srcs "ccomp_timer_impl_riscv.c")
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND srcs "ccomp_timer_impl_xtensa.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES spi_flash idf_test cmock
                    PRIV_REQUIRES perfmon esp_ipc driver)
