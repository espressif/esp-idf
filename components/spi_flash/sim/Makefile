PARTITION_SIM=partition_sim
PARTITION_SIM_LIB=lib$(PARTITION_SIM).a

lib: $(PARTITION_SIM_LIB)

SOURCE_FILES = \
    Flash_Emulator.cpp \
    partition.c

INCLUDE_FLAGS = $(addprefix -I,\
	. \
    ./include \
	../../esp32/include/ \
) 

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage 
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))


$(PARTITION_SIM_LIB): $(OBJ_FILES)
	$(AR) rcs $@ $^

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*) $(OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): $(TEST_PROGRAM) lib

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(PARTITION_SIM_LIB)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean lib
