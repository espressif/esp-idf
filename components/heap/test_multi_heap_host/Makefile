TEST_PROGRAM=test_multi_heap
all: $(TEST_PROGRAM)

SOURCE_FILES = $(abspath \
    ../multi_heap.c \
	../multi_heap_poisoning.c \
	test_multi_heap.cpp \
	main.cpp \
    )

INCLUDE_FLAGS = -I../include -I../../../tools/catch

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g -fstack-protector-all -m32
CFLAGS += -Wall -Werror -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage -m32

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)

$(TEST_PROGRAM): $(OBJ_FILES)
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

$(COVERAGE_FILES): $(TEST_PROGRAM) test

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory $(abspath ../) --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test
