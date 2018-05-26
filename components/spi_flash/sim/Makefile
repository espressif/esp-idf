COMPONENT=spi_flash
COMPONENT_LIB=lib$(COMPONENT).a

all: lib

SOURCE_FILES = \
	../partition.c \
	../flash_ops.c \
	SpiFlash.cpp \
	flash_mock_util.c	\
	flash_mock.cpp \
	$(addprefix stubs/, \
	newlib/lock.c \
	app_update/esp_ota_eps.c \
	)

INCLUDE_FLAGS = $(addprefix -I,\
	. \
	../include \
	../../esp32/include/ \
	$(addprefix stubs/, \
	newlib/include \
	log/include \
	freertos/include \
	) \
	$(addprefix ../../../components/, \
	soc/esp32/include \
	esp32/include \
	bootloader_support/include \
	app_update/include \
	) \
	../../../tools/catch \
)

GCOV ?= gcov

CPPFLAGS += $(INCLUDE_FLAGS) -D CONFIG_LOG_DEFAULT_LEVEL -g -m32
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -ftest-coverage
LDFLAGS += -lstdc++ -fprofile-arcs -ftest-coverage

OBJ_FILES = $(filter %.o, $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))

$(COMPONENT_LIB): $(OBJ_FILES)
	$(AR) rcs $@ $^

lib: $(COMPONENT_LIB)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)

$(COVERAGE_FILES): lib

coverage.info: $(COVERAGE_FILES)
	find ../ -name "*.gcno" -exec $(GCOV) -r -pb {} +
	lcov --capture --directory ../ --no-external --output-file coverage.info --gcov-tool $(GCOV)

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	rm -f $(OBJ_FILES) $(COMPONENT_LIB)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

.PHONY: clean lib all
