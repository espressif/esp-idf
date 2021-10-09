set(component_srcs "src/step_motor.c"
                   "src/step_motor_rmt.c"
                   "src/step_motor_driver_io_a4988.c"
)

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES "")
