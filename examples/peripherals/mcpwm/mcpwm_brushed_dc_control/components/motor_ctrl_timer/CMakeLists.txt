set(COMPONENT_SRCS  "motor_ctrl_timer.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                       INCLUDE_DIRS .
                       PRIV_REQUIRES "driver")
