set(tjpgd_srcs "src/tjpgd.c")

idf_component_register(SRCS "${tjpgd_srcs}"
                       INCLUDE_DIRS "include")
