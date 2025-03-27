idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES esp_driver_i2s
                    INCLUDE_DIRS ".")

# 数学関数（sin関数など）を使うためにリンク
target_link_libraries(${COMPONENT_LIB} INTERFACE m)
