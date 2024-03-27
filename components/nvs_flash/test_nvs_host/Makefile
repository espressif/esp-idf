TEST_PROGRAM=test_nvs
all: $(TEST_PROGRAM)

SOURCE_FILES = \
	$(addprefix ../src/, \
		nvs_types.cpp \
		nvs_api.cpp \
		nvs_page.cpp \
		nvs_pagemanager.cpp \
		nvs_storage.cpp \
		nvs_item_hash_list.cpp \
		nvs_handle_simple.cpp \
		nvs_handle_locked.cpp \
		nvs_partition_manager.cpp \
		nvs_partition.cpp \
		nvs_encrypted_partition.cpp \
		nvs_cxx_api.cpp \
		nvs_platform.cpp \
	) \
	spi_flash_emulation.cpp \
	test_compressed_enum_table.cpp \
	test_spi_flash_emulation.cpp \
	test_intrusive_list.cpp \
	test_nvs.cpp \
	test_nvs_partition.cpp \
	test_partition_manager.cpp \
	main.cpp

SOURCE_FILES_C = ../../esp_rom/linux/esp_rom_crc.c ../../esp_common/src/esp_err_check_linux.c

ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"), 1)
COMPILER := clang
else
COMPILER := gcc
endif

CPPFLAGS += -I../private_include -I../include -I../src -I../../esp_rom/include -I../../esp_rom/include/linux -I../../log/include -I./ -I../../esp_common/include -I../../esp32/include -I ../../mbedtls/mbedtls/include -I ../../spi_flash/include -I ../../esp_partition/include -I ../../hal/include -I ../../xtensa/include -I ../../soc/linux/include -I ../../../tools/catch -fprofile-arcs -ftest-coverage -g2 -ggdb
CFLAGS += -fprofile-arcs -ftest-coverage -DLINUX_TARGET -DLINUX_HOST_LEGACY_TEST
CXXFLAGS += -std=c++11 -Wall -Werror -DLINUX_TARGET -DLINUX_HOST_LEGACY_TEST
LDFLAGS += -lstdc++ -Wall -fprofile-arcs -ftest-coverage

ifeq ($(shell uname -s),Linux)
LDFLAGS += -lbsd
endif

ifeq ($(COMPILER),clang)
CFLAGS += -fsanitize=address
CXXFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address
endif

OBJ_FILES = $(SOURCE_FILES:.cpp=.o)
OBJ_FILES_C = $(SOURCE_FILES_C:.c=.o)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)
MBEDTLS_LIB := ../../mbedtls/mbedtls/library/libmbedcrypto.a

$(OBJ_FILES): %.o: %.cpp
$(OBJ_FILES_C): %.c: %.c

$(MBEDTLS_LIB):
	$(MAKE) -C ../../mbedtls/mbedtls/ lib

$(TEST_PROGRAM): $(OBJ_FILES) $(OBJ_FILES_C) $(MBEDTLS_LIB) | clean-coverage
	g++ -o $@ $^ $(LDFLAGS)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes exclude:[long]

long-test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes

$(COVERAGE_FILES): $(TEST_PROGRAM) long-test

coverage.info: $(COVERAGE_FILES)
	find ../src/ -name "*.gcno" -exec gcov -r -pb {} +
	lcov --capture --directory ../src --no-external --output-file coverage.info

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean-coverage:
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info

clean: clean-coverage
	$(MAKE) -C ../../mbedtls/mbedtls/ clean
	rm -f $(OBJ_FILES) $(OBJ_FILES_C) $(TEST_PROGRAM)
	rm -f ../nvs_partition_generator/partition_single_page.bin
	rm -f ../nvs_partition_generator/partition_multipage_blob.bin
	rm -f ../nvs_partition_generator/partition_encrypted.bin
	rm -f ../nvs_partition_generator/partition_encrypted_hmac.bin
	rm -f ../nvs_partition_generator/partition_encrypted_using_keygen.bin
	rm -f ../nvs_partition_generator/partition_encrypted_using_keyfile.bin
	rm -f ../nvs_partition_generator/partition_encrypted_using_keygen_hmac.bin
	rm -f ../nvs_partition_generator/partition_decrypted.bin
	rm -f ../nvs_partition_generator/partition_decrypted_hmac.bin
	rm -f ../nvs_partition_generator/partition_encoded.bin
	rm -f ../nvs_partition_generator/Test-1-partition-encrypted-hmac.bin
	rm -f ../nvs_partition_generator/Test-1-partition-encrypted.bin
	rm -f ../nvs_partition_generator/Test-1-partition.bin
	rm -f ../../../tools/mass_mfg/samples/sample_values_multipage_blob_created.csv
	rm -f ../../../tools/mass_mfg/samples/sample_values_singlepage_blob_created.csv



.PHONY: clean clean-coverage all test long-test
