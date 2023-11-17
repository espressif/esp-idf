set(src_test "test_heap_main.c"
             "test_aligned_alloc_caps.c"
             "test_allocator_timings.c"
             "test_corruption_check.c"
             "test_diram.c"
             "test_heap_trace.c"
             "test_malloc_caps.c"
             "test_malloc.c"
             "test_realloc.c"
             "test_runtime_heap_reg.c"
             "test_task_tracking.c")

idf_component_register(SRCS ${src_test}
                       INCLUDE_DIRS "."
                       REQUIRES unity esp_psram spi_flash
                       WHOLE_ARCHIVE)
