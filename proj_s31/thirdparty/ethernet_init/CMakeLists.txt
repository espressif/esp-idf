set(priv_requires esp_driver_gpio esp_driver_spi esp_eth yt8531)

idf_component_register(SRCS "ethernet_init.c"
                       PRIV_REQUIRES ${priv_requires}
                       INCLUDE_DIRS ".")
