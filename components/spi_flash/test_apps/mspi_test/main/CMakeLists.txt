set(srcs "test_cache_disabled.c"
         "test_out_of_bounds_write.c"
         "test_read_write.c"
         "test_large_flash_writes.c"
         "test_app_main.c")

idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
