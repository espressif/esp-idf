if(CONFIG_NEWLIB_NANO_FORMAT)
    set(LIBC c_nano)
else()
    set(LIBC c)
endif()

set(LIBM m)
