TEST_PROGRAM=test_gdbstub_rv
GDBSTUB_SRC_DIR=../..
all: $(TEST_PROGRAM)

SOURCE_FILES = \
	$(addprefix $(GDBSTUB_SRC_DIR)/src/, \
		port/riscv/rv_decode.c \
	) \
	test_rv_decode.cpp \
	main.cpp

INCLUDE_FLAGS = -I./include \
                -I$(GDBSTUB_SRC_DIR)/private_include \
                -I$(GDBSTUB_SRC_DIR)/include \
                -I$(GDBSTUB_SRC_DIR)/src/port/riscv/include \
                -I$(GDBSTUB_SRC_DIR)/../../tools/catch \
                -I$(GDBSTUB_SRC_DIR)/../esp_hw_support/include \
                -I$(GDBSTUB_SRC_DIR)/../soc/esp32c3/include \
                -I$(GDBSTUB_SRC_DIR)/../esp_common/include \
                -I$(GDBSTUB_SRC_DIR)/../riscv/include
CPPFLAGS += $(INCLUDE_FLAGS) -D__riscv_c -Wall -Werror -g --coverage
CFLAGS += $(INCLUDE_FLAGS) -D__riscv_c -Wall -Werror -g --coverage
LDFLAGS += -lstdc++ --coverage

ifeq ($(CC),clang)
CFLAGS += -fsanitize=address
CXXFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address
endif

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)

$(TEST_PROGRAM): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes exclude:[long]

long-test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes

$(COVERAGE_FILES): $(TEST_PROGRAM) long-test

coverage.info: $(COVERAGE_FILES)
	find $(GDBSTUB_SRC_DIR)/src/ -name "*.gcno" -exec gcov -r -pb {} +
	lcov --capture --directory $(GDBSTUB_SRC_DIR)/src --output-file coverage.info

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean-coverage:
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

clean: clean-coverage
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)


.PHONY: clean clean-coverage all test long-test
