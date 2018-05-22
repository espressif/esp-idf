TEST_PROGRAM=test_spiffs

TEST_PARTITION_SIM_DIR=$(IDF_PATH)/components/spi_flash/sim
TEST_PARTITION_SIM_LIB=libpartition_sim.a

all: $(TEST_PROGRAM)

SOURCE_FILES = \
	main.cpp \
	test_spiffs.cpp \
	../spiffs_api.c \
	$(addprefix ../spiffs/src/, \
	spiffs_cache.c \
	spiffs_check.c \
	spiffs_gc.c \
	spiffs_hydrogen.c \
	spiffs_nucleus.c \
	) \
	$(addprefix ./stubs/, \
	log/log.c \
	)

INCLUDE_FLAGS = $(addprefix -I,\
	. \
	.. \
	../spiffs/src \
	../include \
	$(addprefix ./stubs/, \
	esp32/include \
	log/include \
	freertos/include \
	newlib/include \
	vfs/include \
	) \
	../../esp32/include \
	$(TEST_PARTITION_SIM_DIR)/include \
	../../../tools/catch \
)

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))

$(TEST_PARTITION_SIM_DIR)/$(TEST_PARTITION_SIM_LIB): force
	$(MAKE) -C $(TEST_PARTITION_SIM_DIR) lib

$(TEST_PROGRAM): $(OBJ_FILES) $(TEST_PARTITION_SIM_DIR)/$(TEST_PARTITION_SIM_LIB)
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES) -L$(TEST_PARTITION_SIM_DIR) -l:$(TEST_PARTITION_SIM_LIB) -L$(TEST_WL_DIR) -l:$(TEST_WL_LIB)

force:

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*) $(OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): $(TEST_PROGRAM) lib

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(TEST_PROGRAM) $(TEST_WL_LIB)
	$(MAKE) -C $(TEST_PARTITION_SIM_DIR) clean
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test
