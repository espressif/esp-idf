spaces2list(ULP_S_SOURCES)
spaces2list(ULP_EXP_DEP_SRCS)

foreach(ulp_s_source ${ULP_S_SOURCES})
    get_filename_component(ulp_src ${ulp_s_source} ABSOLUTE BASE_DIR ${COMPONENT_DIR})
    list(APPEND ulp_srcs ${ulp_src})
endforeach()

foreach(ulp_exp_dep_src ${ULP_EXP_DEP_SRCS})
    get_filename_component(ulp_dep_src ${ulp_exp_dep_src} ABSOLUTE BASE_DIR ${COMPONENT_DIR})
    list(APPEND ulp_dep_srcs ${ulp_dep_src})
endforeach()

ulp_embed_binary(${ULP_APP_NAME} "${ulp_srcs}" "${ulp_dep_srcs}")
