set(COMPONENT_SRCS)
set(COMPONENT_ADD_INCLUDEDIRS ".")

idf_component_register(SRCS "semihost_vfs_example_main.c"
                       PRIV_REQUIRES vfs
                       INCLUDE_DIRS ".")
