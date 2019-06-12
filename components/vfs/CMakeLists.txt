set(COMPONENT_SRCS "vfs.c"
                   "vfs_uart.c"
                   "vfs_semihost.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
register_component()

# Some newlib syscalls are implemented in vfs.c, make sure these are always
# seen by the linker
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u vfs_include_syscalls_impl")
