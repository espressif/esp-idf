set(srcs "esp_tee_flash.c")
set(include_dirs  "include")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES esp_tee log
                       REQUIRES bootloader_support)
