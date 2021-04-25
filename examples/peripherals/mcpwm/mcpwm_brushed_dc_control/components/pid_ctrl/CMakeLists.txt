set(COMPONENT_SRCS  "pid_ctrl.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                       INCLUDE_DIRS .
                       PRIV_REQUIRES "driver")
