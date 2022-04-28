COMPONENT_PRIV_INCLUDEDIRS := private_include
COMPONENT_ADD_INCLUDEDIRS : include
COMPONENT_SRCDIRS := .
COMPONENT_OBJS := ccomp_timer.o test_runner.o test_utils.o

ifdef CONFIG_IDF_TARGET_ESP32
COMPONENT_OBJS += ref_clock_impl_rmt_pcnt.o
else
COMPONENT_OBJS += ref_clock_impl_timergroup.o
endif

ifdef CONFIG_IDF_TARGET_ARCH_RISCV
COMPONENT_OBJS += ccomp_timer_impl_riscv.o
endif

ifdef CONFIG_IDF_TARGET_ARCH_XTENSA
COMPONENT_OBJS += ccomp_timer_impl_xtensa.o
endif
