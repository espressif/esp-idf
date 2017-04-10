TEST_PROGRAM=test_wl
all: $(TEST_PROGRAM)

SOURCE_FILES = \
	esp_error_check_stub.cpp \
	$(addprefix ../, \
		crc32.cpp \
		WL_Flash.cpp \
		../nvs_flash/test_nvs_host/crc.cpp\
	) \
	Flash_Emulator.cpp \
	wl_tests_host.cpp  \
	TestPowerDown.cpp  \
	esp_log_stub.cpp \
	main.cpp


INCLUDE_FLAGS = $(addprefix -I,\
	../ \
	../include \
	../private_include \
	../../esp32/include \
	../../soc/esp32/include \
	../../log/include \
	../../spi_flash/include \
	../../nvs_flash/test_nvs_host \
	../../../tools/catch \
) 

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

OBJ_FILES = $(SOURCE_FILES:.cpp=.o)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)

$(OBJ_FILES): %.o: %.cpp

$(TEST_PROGRAM): $(OBJ_FILES)
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

$(COVERAGE_FILES): $(TEST_PROGRAM) test

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean all test
