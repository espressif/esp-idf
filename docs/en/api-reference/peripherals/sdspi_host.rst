SD SPI Host Driver
==================

Overview
--------

SPI controllers accessible via spi_master driver (HSPI, VSPI) can be used to work with SD cards. The driver which provides this capability is called "SD SPI Host", due to its similarity with the :doc:`SDMMC Host <sdmmc_host>` driver.

In SPI mode, SD driver has lower throughput than in 1-line SD mode. However SPI mode makes pin selection more flexible, as SPI peripheral can be connected to any ESP32 pins using GPIO Matrix. SD SPI driver uses software controlled CS signal. Currently SD SPI driver assumes that it can use the SPI controller exclusively, so applications which need to share SPI bus between SD cards and other peripherals need to make sure that SD card and other devices are not used at the same time from different tasks. 

SD SPI driver is represented using an :cpp:class:`sdmmc_host_t` structure initialized using :c:macro:`SDSPI_HOST_DEFAULT` macro. For slot initialization, :c:macro:`SDSPI_SLOT_CONFIG_DEFAULT` can be used to fill in default pin mapping, which is the same as the pin mapping in SD mode.

SD SPI driver APIs are very similar to :doc:`SDMMC host APIs <sdmmc_host>`. As with the SDMMC host driver, only :cpp:func:`sdspi_host_init`, :cpp:func:`sdspi_host_init_slot`, and :cpp:func:`sdspi_host_deinit` functions are normally used by the applications. Other functions are called by the protocol level driver via function pointers in :cpp:class:`sdmmc_host_t` structure.

.. note:
	
	SD over SPI does not support speeds above SDMMC_FREQ_DEFAULT due to a limitation of SPI driver.

See :doc:`SD/SDIO/MMC Driver <../storage/sdmmc>` for the higher level driver which implements the protocol layer.


API Reference
-------------

.. include:: /_build/inc/sdspi_host.inc
