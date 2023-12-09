set(srcs "sdmmc_test_cd_wp_common.c" "sdmmc_test_rw_common.c")

set(public_include "include")

idf_component_register(
     SRCS ${srcs}
     INCLUDE_DIRS ${public_include}
     PRIV_REQUIRES sdmmc unity test_utils
)
