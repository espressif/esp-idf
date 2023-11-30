set(srcs "test_app_main.c")

set(priv_requires
        # tests reside in this component, also available for `sdmmc_console`
        sdspi_tests
        # general
        unity
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
