ifndef COMPONENT
COMPONENT := spiffs
endif

COMPONENT_LIB := lib$(COMPONENT).a
TEST_PROGRAM := test_$(COMPONENT)

STUBS_LIB_DIR := ../../../components/spi_flash/sim/stubs
STUBS_LIB_BUILD_DIR := $(STUBS_LIB_DIR)/build
STUBS_LIB := libstubs.a

SPI_FLASH_SIM_DIR := ../../../components/spi_flash/sim
SPI_FLASH_SIM_BUILD_DIR := $(SPI_FLASH_SIM_DIR)/build
SPI_FLASH_SIM_LIB := libspi_flash.a

include Makefile.files

all: test

ifndef SDKCONFIG
SDKCONFIG_DIR := $(dir $(realpath sdkconfig/sdkconfig.h))
SDKCONFIG := $(SDKCONFIG_DIR)sdkconfig.h
else
SDKCONFIG_DIR := $(dir $(realpath $(SDKCONFIG)))
endif

INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS) $(SDKCONFIG_DIR) ../../../tools/catch)

CPPFLAGS += $(INCLUDE_FLAGS) -g -m32
CXXFLAGS += $(INCLUDE_FLAGS) -std=c++11 -g -m32

# Build libraries that this component is dependent on
$(STUBS_LIB_BUILD_DIR)/$(STUBS_LIB): force
	$(MAKE) -C $(STUBS_LIB_DIR) lib SDKCONFIG=$(SDKCONFIG)

$(SPI_FLASH_SIM_BUILD_DIR)/$(SPI_FLASH_SIM_LIB): force
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) lib SDKCONFIG=$(SDKCONFIG)

# Create target for building this component as a library
CFILES := $(filter %.c, $(SOURCE_FILES))
CPPFILES := $(filter %.cpp, $(SOURCE_FILES))

CTARGET = ${2}/$(patsubst %.c,%.o,$(notdir ${1}))
CPPTARGET = ${2}/$(patsubst %.cpp,%.o,$(notdir ${1}))

ifndef BUILD_DIR
BUILD_DIR := build
endif

OBJ_FILES := $(addprefix $(BUILD_DIR)/, $(filter %.o, $(notdir $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))))

define COMPILE_C
$(call CTARGET, ${1}, $(BUILD_DIR)) : ${1} $(SDKCONFIG)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(call CTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

define COMPILE_CPP
$(call CPPTARGET, ${1}, $(BUILD_DIR)) : ${1} $(SDKCONFIG)
	mkdir -p $(BUILD_DIR) 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(call CPPTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

$(BUILD_DIR)/$(COMPONENT_LIB): $(OBJ_FILES) $(SDKCONFIG)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

clean:
	$(MAKE) -C $(STUBS_LIB_DIR) clean
	$(MAKE) -C $(SPI_FLASH_SIM_DIR) clean
	rm -f $(OBJ_FILES) $(TEST_OBJ_FILES) $(TEST_PROGRAM) $(COMPONENT_LIB) partition_table.bin

lib: $(BUILD_DIR)/$(COMPONENT_LIB)

$(foreach cfile, $(CFILES), $(eval $(call COMPILE_C, $(cfile))))
$(foreach cxxfile, $(CPPFILES), $(eval $(call COMPILE_CPP, $(cxxfile))))

# Create target for building this component as a test
TEST_SOURCE_FILES = \
	test_spiffs.cpp \
	main.cpp \
	test_utils.c

TEST_OBJ_FILES = $(filter %.o, $(TEST_SOURCE_FILES:.cpp=.o) $(TEST_SOURCE_FILES:.c=.o))

$(TEST_PROGRAM): lib $(TEST_OBJ_FILES) $(SPI_FLASH_SIM_BUILD_DIR)/$(SPI_FLASH_SIM_LIB) $(STUBS_LIB_BUILD_DIR)/$(STUBS_LIB) partition_table.bin $(SDKCONFIG)
	g++ $(LDFLAGS) $(CXXFLAGS) -o $@  $(TEST_OBJ_FILES) -L$(BUILD_DIR) -l:$(COMPONENT_LIB) -L$(SPI_FLASH_SIM_BUILD_DIR) -l:$(SPI_FLASH_SIM_LIB) -L$(STUBS_LIB_BUILD_DIR) -l:$(STUBS_LIB)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

# Create other necessary targets
partition_table.bin: partition_table.csv
	python ../../../components/partition_table/gen_esp32part.py --verify $< $@

force:

.PHONY: all lib test clean force
