COMPONENT=fatfs

TEST_PROGRAM=test_$(COMPONENT)

#Expose as a library
COMPONENT_LIB=lib$(COMPONENT).a

#Use wear levelling and flash simulation
WEAR_LEVELLING=wear_levelling
WEAR_LEVELLING_DIR=../../$(WEAR_LEVELLING)
WEAR_LEVELLING_HOST_DIR=$(WEAR_LEVELLING_DIR)/test_wl_host
WEAR_LEVELLING_LIB=lib$(WEAR_LEVELLING).a

SPI_FLASH=spi_flash
SPI_FLASH_DIR=../../$(SPI_FLASH)
SPI_FLASH_SIM_DIR=$(SPI_FLASH_DIR)/sim
SPI_FLASH_LIB=lib$(SPI_FLASH).a

all: $(TEST_PROGRAM)

SOURCE_FILES = \
	$(addprefix ../src/, \
	diskio.c \
	ff.c \
	ffsystem.c \
	ffunicode.c \
	diskio_wl.c \
	) \
	$(addprefix ./stubs/, log/log.c)

TEST_SOURCE_FILES = \
	test_fatfs.cpp \
	main.cpp \
	test_utils.c

INCLUDE_FLAGS = $(addprefix -I,\
	../src \
	. \
	$(addprefix ./stubs/, \
	driver/include \
	freertos/include \
	sdmmc/include \
	log/include \
	) \
	$(SPI_FLASH_DIR)/include \
	$(WEAR_LEVELLING_DIR)/include \
	../../esp32/include \
	../../../tools/catch \
)

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g -m32
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))
TEST_OBJ_FILES = $(filter %.o, $(TEST_SOURCE_FILES:.cpp=.o) $(TEST_SOURCE_FILES:.c=.o))

$(WEAR_LEVELLING_HOST_DIR)/$(WEAR_LEVELLING_LIB): force
	$(MAKE) -C $(WEAR_LEVELLING_HOST_DIR) lib

$(SPI_FLASH_SIM_DIR)/$(SPI_FLASH_LIB): force
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) lib

force:

$(COMPONENT_LIB): $(OBJ_FILES)
	$(AR) rcs $@ $^

lib: $(COMPONENT_LIB)

partition_table.bin: partition_table.csv
	python ../../partition_table/gen_esp32part.py $< $@

$(TEST_PROGRAM): lib $(TEST_OBJ_FILES) $(SPI_FLASH_SIM_DIR)/$(SPI_FLASH_LIB) $(WEAR_LEVELLING_HOST_DIR)/$(WEAR_LEVELLING_LIB) partition_table.bin
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(TEST_OBJ_FILES) -L$(abspath .) -l:$(COMPONENT_LIB) -L$(SPI_FLASH_SIM_DIR) -l:$(SPI_FLASH_LIB) -L$(WEAR_LEVELLING_HOST_DIR) -l:$(WEAR_LEVELLING_LIB) -g -m32

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*) $(TEST_OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): test

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ_FILES) $(TEST_PROGRAM) $(COMPONENT_LIB) partition_table.bin
	$(MAKE) -C $(WEAR_LEVELLING_HOST_DIR) clean
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) clean
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test lib
