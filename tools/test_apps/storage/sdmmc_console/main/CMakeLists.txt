set(requires
        # for console functionality
        console
        # for console history
        vfs fatfs
        # sdmmc tests, themselves
        sdmmc_test_boards sdmmc_tests sdspi_tests
        # various console commands
        cmd_unity cmd_system cmd_sdmmc
        # for PSRAM-related tests
        esp_psram
)

idf_component_register(SRCS sdmmc_console_main.c
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${requires})
