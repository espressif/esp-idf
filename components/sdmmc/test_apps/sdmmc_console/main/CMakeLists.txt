set(requires
        # for console functionality
        console
        # for console history
        vfs fatfs
        # sdmmc tests, themselves
        sdmmc_test_board sdmmc_tests
        # various console comamnds
        cmd_unity cmd_system cmd_sdmmc
)

idf_component_register(SRCS sdmmc_console_main.c
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${requires})
