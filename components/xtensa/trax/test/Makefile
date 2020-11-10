PROG_BIN := test.bin
PROG_ELF := test.elf
OBJ := test.o
PREFIX := xtensa-esp32-elf-
CC := $(PREFIX)gcc
LD := $(PREFIX)ld
GDB ?= $(PREFIX)gdb
CFLAGS := -g -O0
LDFLAGS := -L $(IDF_PATH)/components/bootloader/subproject/main/ld/esp32 \
	-T bootloader.ld \
	-u entry \
	-e entry \
	-g

TRACE_FILE := trace_pro.bin
GDB_LOG := gdb_log.txt
GDB_LOG_REF := gdb_log_expected.txt

all: $(PROG_BIN)

$(PROG_ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $<

$(PROG_BIN): $(PROG_ELF)
	esptool.py --chip esp32 elf2image --output $@ --flash_freq 40m --flash_mode dio $<

flash: $(PROG_BIN)
	esptool.py --port $(ESPPORT) write_flash 0x1000 $(PROG_BIN)

test:
	rm -f $(TRACE_FILE)
	$(GDB) -n --batch -x gdbinit | tee $(GDB_LOG)
	diff $(GDB_LOG) $(GDB_LOG_REF)
	@echo "Test pass"

clean:
	rm -f $(PROG_ELF) $(PROG_BIN) $(OBJ) $(TRACE_FILE) $(GDB_LOG)

.PHONY: all clean flash test
