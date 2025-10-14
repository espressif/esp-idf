idf_component_register(SRCS "test_fatfs_dyn_buffers.c"
                    INCLUDE_DIRS "."
                    REQUIRES wear_levelling fatfs vfs)
