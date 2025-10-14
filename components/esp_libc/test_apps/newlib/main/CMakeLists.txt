set(srcs
    "test_app_main.c"
    "test_atomic.c"
    "test_locks.c"
    "test_misc.c"
    "test_memstream.c"
    "test_newlib.c"
    "test_printf.c"
    "test_setjmp.c"
    "test_stdatomic.c"
    "test_time.c")

if(CONFIG_LIBC_NEWLIB)
    list(APPEND srcs
        # test_file.c tests if "_blksize" field was set correct to "FILE" struct.
        # But picolibc does not have such field in the "FILE". So test is applicable only for newlib.
         "test_file.c")
endif()

if(CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS)
    list(APPEND srcs "test_misaligned_access.c")
endif()

idf_component_register(SRCS "${srcs}"
                       PRIV_REQUIRES unity vfs cmock esp_timer spi_flash test_utils pthread esp_psram
                       WHOLE_ARCHIVE)
