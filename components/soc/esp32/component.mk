ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += esp32/emac_hal.o
endif

esp32/rtc_clk.o: CFLAGS += -fno-jump-tables -fno-tree-switch-conversion
