set(srcs "main.c"
    "board.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
