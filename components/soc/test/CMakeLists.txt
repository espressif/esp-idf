idf_build_get_property(soc_name IDF_TARGET)

idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES unity test_utils)
