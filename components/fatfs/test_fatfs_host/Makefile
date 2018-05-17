TEST_PROGRAM=fatfs_host

all: $(TEST_PROGRAM)

SOURCE_FILES = \
	main.c \
	$(addprefix ../src/, \
	diskio.c \
	ff.c \
	ffsystem.c \
    ffunicode.c \
	)

INCLUDE_FLAGS = $(addprefix -I,\
	../src \
    . \
    $(addprefix ./stubs/, \
    driver/include \
    freertos/include \
    sdmmc/include \
    ) \
    ../../esp32/include \
)

CPPFLAGS += $(INCLUDE_FLAGS) -g
CFLAGS += -fprofile-arcs  -g
CXXFLAGS += -std=c++11 -Wall -Werror  -fprofile-arcs -g
LDFLAGS += -lstdc++ -fprofile-arcs

OBJ_FILES = $(SOURCE_FILES:.c=.o)
$(OBJ_FILES): %.o: %.c

$(TEST_PROGRAM): $(OBJ_FILES)
	gcc $(LDFLAGS)  -o $(TEST_PROGRAM) $(OBJ_FILES) 

$(TEST_PROGRAM): $(OBJ_FILES) $(TEST_WL_DIR)/$(TEST_WL_LIB) $(TEST_PARTITION_SIM_DIR)/$(TEST_PARTITION_SIM_LIB) 
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES) -L$(TEST_PARTITION_SIM_DIR) -l:$(TEST_PARTITION_SIM_LIB) -L$(TEST_WL_DIR) -l:$(TEST_WL_LIB)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

clean: 
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)

.PHONY: clean all
