idf_component_register(SRCS "umode_elf.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_privilege
                       PRIV_REQUIRES heap)
