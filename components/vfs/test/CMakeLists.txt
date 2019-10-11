idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES unity test_utils vfs fatfs spiffs
                    LDFRAGMENTS linker.lf)