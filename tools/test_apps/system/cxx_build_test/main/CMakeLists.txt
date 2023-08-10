idf_component_register(SRCS cxx_build_test_main.cpp
                            test_soc_reg_macros.cpp
                            test_cxx_standard.cpp
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver
                       REQUIRES soc)
