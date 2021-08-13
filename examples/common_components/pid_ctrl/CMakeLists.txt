set(srcs "src/pid_ctrl.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include")
