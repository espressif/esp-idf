# idf_component_register(SRCS "sht21.c"
#                     INCLUDE_DIRS ".")

                    idf_component_register(SRCS "sht21.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver)
