set(srcs "main.c"
        "misc.c"
        "scli.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".")
