idf_build_get_property(soc_name IDF_TARGET)

idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES unity test_utils)
