# ulp_clamp is resolved by the component manager via idf_component.yml
idf_component_register(SRCS "ulp_sum.c"
                       INCLUDE_DIRS "include"
                       REQUIRES ulp_clamp)
