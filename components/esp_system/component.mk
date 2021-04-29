ifdef IS_BOOTLOADER_BUILD

ifndef CONFIG_IDF_ENV_FPGA
COMPONENT_CONFIG_ONLY := 1
else
COMPONENT_SRCDIRS := .
COMPONENT_OBJS += fpga_overrides.o
endif

else
SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := .
COMPONENT_ADD_INCLUDEDIRS := include port/public_compat
COMPONENT_PRIV_INCLUDEDIRS := port/include port
COMPONENT_ADD_LDFRAGMENTS += linker.lf app.lf

ifndef CONFIG_IDF_ENV_FPGA
COMPONENT_OBJEXCLUDE += fpga_overrides.o
endif

# Force linking UBSAN hooks. If UBSAN is not enabled, the hooks will ultimately be removed
# due to -ffunction-sections -Wl,--gc-sections options.
COMPONENT_ADD_LDFLAGS += -u __ubsan_include

include $(COMPONENT_PATH)/port/soc/$(SOC_NAME)/component.mk

# disable stack protection in files which are involved in initialization of that feature
startup.o stack_check.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))
endif
