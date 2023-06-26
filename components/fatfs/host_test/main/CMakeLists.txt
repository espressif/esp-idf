idf_component_register(SRCS "main.cpp"
                            "test_fatfs.cpp"
                       INCLUDE_DIRS "$ENV{IDF_PATH}/tools/catch"
                       REQUIRES fatfs
                       WHOLE_ARCHIVE
                       )
