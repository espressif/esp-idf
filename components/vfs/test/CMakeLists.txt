set(srcdirs ".")

idf_component_register(SRC_DIRS ${srcdirs}
                       INCLUDE_DIRS .
                       REQUIRES unity test_utils vfs fatfs spiffs
                       LDFRAGMENTS linker.lf
                      )
