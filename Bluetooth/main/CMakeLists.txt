# idf_component_register(SRCS "main.c"
#                           "uart_driver.c"
#                     INCLUDE_DIRS ".")
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
