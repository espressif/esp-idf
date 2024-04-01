set(srcs)

list(APPEND srcs "src/isp_af_scheme.c"
                 "src/isp_af_scheme_sa.c")

set(priv_requires "esp_driver_isp")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" "interface"
                       PRIV_REQUIRES ${priv_requires})
