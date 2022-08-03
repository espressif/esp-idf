set(srcs "esp_hid_host_main.c"
         "esp_hid_gap.c")
set(include_dirs ".")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       REQUIRES esp_hid
                       PRIV_REQUIRES nvs_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
