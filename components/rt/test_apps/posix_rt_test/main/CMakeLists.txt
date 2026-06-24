idf_component_register(SRCS "main.c" "mqueue_test.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES rt unity
                    WHOLE_ARCHIVE)
