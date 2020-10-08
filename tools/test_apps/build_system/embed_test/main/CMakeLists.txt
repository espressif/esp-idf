idf_component_register(SRCS "test_main.c"
                       INCLUDE_DIRS "."
                       # Test file names starting with a number, a letter and an underscore.
                       EMBED_TXTFILES "2file.txt" "file.txt" "_file.txt")
