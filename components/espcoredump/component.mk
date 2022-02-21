COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src
COMPONENT_PRIV_INCLUDEDIRS := include_core_dump
COMPONENT_ADD_LDFRAGMENTS += linker.lf

ifdef CONFIG_IDF_TARGET_ARCH_XTENSA
	COMPONENT_SRCDIRS += src/port/xtensa
	COMPONENT_ADD_INCLUDEDIRS += include/port/xtensa
	COMPONENT_PRIV_INCLUDEDIRS += include_core_dump/port/xtensa
endif

ifdef CONFIG_IDF_TARGET_ARCH_RISCV
	COMPONENT_SRCDIRS += src/port/riscv
	COMPONENT_ADD_INCLUDEDIRS += include/port/riscv
	COMPONENT_PRIV_INCLUDEDIRS += include_core_dump/port/riscv
endif
