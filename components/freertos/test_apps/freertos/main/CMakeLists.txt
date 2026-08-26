idf_component_register(SRCS "test_freertos_main.c"
                       # Pull in the components containing each type of FreeRTOS test
                       PRIV_REQUIRES unity test_utils kernel misc performance port)
