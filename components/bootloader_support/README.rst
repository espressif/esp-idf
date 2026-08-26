Bootloader Support Component
============================

Overview
--------

"Bootloader support" contains APIs which are used by the bootloader but are also needed for the main app.

Code in this component needs to be aware of being executed in a bootloader environment (no RTOS available, BOOTLOADER_BUILD macro set) or in an esp-idf app environment (RTOS running, need locking support.)
