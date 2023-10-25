idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "ringbuf.c"
                    INCLUDE_DIRS "include"
                    LDFRAGMENTS linker.lf)
