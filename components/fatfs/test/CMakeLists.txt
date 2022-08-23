idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES cmock test_utils vfs fatfs
                       EMBED_TXTFILES fatfs.img
                      )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
