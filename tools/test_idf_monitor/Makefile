# The purpose of this Makefile is to build dummy ELF files required to run idf_monitor tests.

# Make sure the toolchains are in the PATH:
PREFIX_XTENSA ?= xtensa-esp32-elf-
PREFIX_RISCV ?= riscv32-esp-elf-

PROG_XTENSA := dummy_xtensa.elf
PROG_RISCV := dummy_riscv.elf

CPPFLAGS_XTENSA := -DTASK_NAME_OFFSET=52
CPPFLAGS_RISCV := -DTASK_NAME_OFFSET=52

all: $(PROG_XTENSA) $(PROG_RISCV)

$(PROG_XTENSA): dummy.c
	$(PREFIX_XTENSA)gcc $(CPPFLAGS_XTENSA) --specs=nosys.specs -o $@ -g $^
	chmod -x $@

# ^ chmod is there so that we don't have to add ELF files to executables list

$(PROG_RISCV): dummy.c
	$(PREFIX_RISCV)gcc $(CPPFLAGS_RISCV) --specs=nosys.specs -o $@ -g $^
	chmod -x $@

clean:
	rm -f $(PROG_XTENSA) $(PROG_RISCV)

.PHONY: clean all
