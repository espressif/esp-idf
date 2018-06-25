COMPONENT=wear_levelling

TEST_PROGRAM=test_$(COMPONENT)

# Expose as a library for FS components that require wear_levelling
COMPONENT_LIB=lib$(COMPONENT).a

# Use simulated block device
SPI_FLASH=spi_flash
SPI_FLASH_DIR=../../$(SPI_FLASH)
SPI_FLASH_SIM_DIR=$(SPI_FLASH_DIR)/sim
SPI_FLASH_LIB=lib$(SPI_FLASH).a

all: $(TEST_PROGRAM)

SOURCE_FILES = \
	$(addprefix ../, \
	wear_levelling.cpp \
	crc32.cpp \
	WL_Flash.cpp \
	Partition.cpp \
	) \
	$(addprefix stubs/, \
	newlib/lock.c \
	log/log.c \
	esp32/crc.cpp \
	)

TEST_SOURCE_FILES = \
	test_wl.cpp \
	main.cpp \
	test_utils.c

INCLUDE_FLAGS = $(addprefix -I,\
	. \
	../ \
	../include \
	../private_include \
	$(addprefix stubs/, \
	newlib/include \
	log/include \
	) \
	$(SPI_FLASH_DIR)/include \
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

$(SPI_FLASH_SIM_DIR)/$(SPI_FLASH_LIB): force
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) lib

$(COMPONENT_LIB): $(OBJ_FILES)
	$(AR) rcs $@ $^

force:

lib: $(COMPONENT_LIB)

partition_table.bin: partition_table.csv
	python ../../../components/partition_table/gen_esp32part.py $< $@

$(TEST_PROGRAM): lib $(TEST_OBJ_FILES) $(SPI_FLASH_SIM_DIR)/$(SPI_FLASH_LIB) partition_table.bin
	g++ $(LDFLAGS) -o $@  $(TEST_OBJ_FILES) -L$(abspath .) -l:$(COMPONENT_LIB) -L$(SPI_FLASH_SIM_DIR) -l:$(SPI_FLASH_LIB) -g -m32

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*) $(TEST_OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): test

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) clean
	rm -f $(OBJ_FILES) $(TEST_OBJ_FILES) $(TEST_PROGRAM) $(COMPONENT_LIB) partition_table.bin
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test lib
