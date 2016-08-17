TEST_PROGRAM=test_nvs
all: test

SOURCE_FILES = \
	$(addprefix ../src/, \
		nvs_types.cpp \
		nvs_api.cpp \
		nvs_page.cpp \
		nvs_pagemanager.cpp \
		nvs_storage.cpp \
	) \
	spi_flash_emulation.cpp \
	test_compressed_enum_table.cpp \
	test_spi_flash_emulation.cpp \
	test_intrusive_list.cpp \
	test_nvs.cpp \
    crc.cpp \
	main.cpp

CPPFLAGS += -I../include -I../src -I./ -I../../esp32/include -I ../../spi_flash/include
CXXFLAGS += -std=c++11 -Wall -Werror
LDFLAGS += -lstdc++

OBJ_FILES = $(SOURCE_FILES:.cpp=.o)

$(OBJ_FILES): %.o: %.cpp

$(TEST_PROGRAM): $(OBJ_FILES)
	gcc $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

clean:
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)

.PHONY: clean all test
