idf_build_get_property(target IDF_TARGET)

set(srcs "test_flash_mmap.c")

idf_component_register(SRCS ${srcs})
