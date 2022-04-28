set(component_srcs "src/musical_buzzer_rmt.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES driver)
