Storage
=======

SPI Flash Driver
^^^^^^^^^^^^^^^^

XMC-C series flash suspend support has been removed. According to feedback from the flash manufacturer, in some situations the XMC-C flash would require a 1ms interval between resume and next command. This is too long for a software request. Based on the above reason, in order to use suspend safely, we decide to remove flash suspend support from XMC-C series. But you can still force enable it via `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND`. If you have any questions, please contact espressif business support.
