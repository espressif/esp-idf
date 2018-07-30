ifndef COMPONENT
COMPONENT := spi_flash
endif

COMPONENT_LIB := lib$(COMPONENT).a

include Makefile.files

all: lib

ifndef SDKCONFIG
SDKCONFIG_DIR := $(dir $(realpath sdkconfig/sdkconfig.h))
SDKCONFIG := $(SDKCONFIG_DIR)sdkconfig.h
else
SDKCONFIG_DIR := $(dir $(realpath $(SDKCONFIG)))
endif

INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS) $(SDKCONFIG_DIR))

CPPFLAGS += $(INCLUDE_FLAGS) -g -m32
CXXFLAGS += $(INCLUDE_FLAGS) -std=c++11 -g -m32

CFILES := $(filter %.c, $(SOURCE_FILES))
CPPFILES := $(filter %.cpp, $(SOURCE_FILES))

CTARGET = ${2}/$(patsubst %.c,%.o,$(notdir ${1}))
CPPTARGET = ${2}/$(patsubst %.cpp,%.o,$(notdir ${1}))

ifndef BUILD_DIR
BUILD_DIR := build
endif

OBJ_FILES := $(addprefix $(BUILD_DIR)/, $(filter %.o, $(notdir $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))))

%.o : %.c $(SDKCONFIG)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< - o $@

%.o : %.cpp $(SDKCONFIG)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< - o $@

define COMPILE_C
$(call CTARGET, ${1}, $(BUILD_DIR)) : ${1}
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(call CTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

define COMPILE_CPP
$(call CPPTARGET, ${1}, $(BUILD_DIR)) : ${1}
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(call CPPTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

$(BUILD_DIR)/$(COMPONENT_LIB): $(OBJ_FILES) $(SDKCONFIG)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJ_FILES) $(BUILD_DIR)/$(COMPONENT_LIB)

lib: $(BUILD_DIR)/$(COMPONENT_LIB)

$(foreach cfile, $(CFILES), $(eval $(call COMPILE_C, $(cfile))))
$(foreach cxxfile, $(CPPFILES), $(eval $(call COMPILE_CPP, $(cxxfile))))

.PHONY: all lib clean
