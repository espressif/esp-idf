set(src "test_vfs.c" "test_vfs_linux_dev.c")

idf_component_register(SRCS ${src}
                       INCLUDE_DIRS .
                       PRIV_REQUIRES vfs unity
                       WHOLE_ARCHIVE
                       )

# dlsym is used to dynamically link native Linux platform functions during test execution
target_link_libraries(${COMPONENT_LIB} PRIVATE dl)
