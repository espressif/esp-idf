if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND src_dirs .)
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock xtensa perfmon)
