idf_component_register(SRCS "main.cpp"
                            "test_wl.cpp"
                       INCLUDE_DIRS "$ENV{IDF_PATH}/tools/catch"
                       PRIV_INCLUDE_DIRS "../../private_include"
                            "../.."
                       REQUIRES wear_levelling
                       WHOLE_ARCHIVE
                       )
