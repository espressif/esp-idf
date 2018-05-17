TEST_PROGRAM=test_wl

# Expose as a library for FS components that require wear_levelling
TEST_LIB=lib$(TEST_PROGRAM).a

# Use simulated block device
TEST_PARTITION_SIM_DIR=$(IDF_PATH)/components/spi_flash/sim
TEST_PARTITION_SIM_LIB=libpartition_sim.a

all: $(TEST_PROGRAM)

LIB_SOURCE_FILES = \
	$(addprefix stubs/, \
	newlib/lock.c \
	log/log.c \
	esp32/crc.cpp \
	) \
	$(addprefix ../, \
	wear_levelling.cpp \
	crc32.cpp \
	WL_Flash.cpp \
	Partition.cpp \
	) \

TEST_SOURCE_FILES = \
	test_wl.cpp \
	main.cpp

INCLUDE_FLAGS = $(addprefix -I,\
	. \
	../ \
	../include \
	../private_include \
	$(addprefix stubs/, \
	esp32/include \
	newlib/include \
	log/include \
	) \
	../../esp32/include \
	../../spi_flash/sim/include \
	../../../tools/catch \
)

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

LIB_OBJ_FILES = $(filter %.o, $(LIB_SOURCE_FILES:.cpp=.o) $(LIB_SOURCE_FILES:.c=.o))
TEST_OBJ_FILES = $(filter %.o, $(TEST_SOURCE_FILES:.cpp=.o) $(TEST_SOURCE_FILES:.c=.o))

$(TEST_PARTITION_SIM_DIR)/$(TEST_PARTITION_SIM_LIB): force
	$(MAKE) -C $(TEST_PARTITION_SIM_DIR) lib

$(TEST_LIB): $(LIB_OBJ_FILES)
	$(AR) rcs $@ $^

lib: $(TEST_LIB)

$(TEST_PROGRAM): lib $(TEST_OBJ_FILES) $(TEST_PARTITION_SIM_DIR)/$(TEST_PARTITION_SIM_LIB)
	g++ $(LDFLAGS) -o $@  $(TEST_OBJ_FILES) -L$(abspath .) -l:$(TEST_LIB) -L$(TEST_PARTITION_SIM_DIR) -l:$(TEST_PARTITION_SIM_LIB)

force:

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

COVERAGE_FILES = $(LIB_OBJ_FILES:.o=.gc*) $(TEST_OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): $(TEST_PROGRAM) test

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	$(MAKE) -C $(TEST_PARTITION_SIM_DIR) clean
	rm -f $(LIB_OBJ_FILES) $(TEST_OBJ_FILES) $(TEST_PROGRAM) $(TEST_LIB)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test lib
