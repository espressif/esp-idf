idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES sample.xml
                       PRIV_REQUIRES tinyxml2 fatfs)
