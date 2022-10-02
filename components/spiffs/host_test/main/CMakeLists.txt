idf_component_register(SRCS "host_test_spiffs.c"
                       PRIV_INCLUDE_DIRS "../.." "../../spiffs/src"
                       REQUIRES spiffs unity)
