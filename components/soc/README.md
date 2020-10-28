## `soc` ##

The `soc` component provides hardware description for targets supported by ESP-IDF.

    - `xxx_reg.h`   - defines registers related to the hardware
    - `xxx_struct.h` - hardware description in C `struct`
    - `xxx_channel.h` - definitions for hardware with multiple channels
    - `xxx_caps.h`  - features/capabilities of the hardware
    - `xxx_pins.h`  - pin definitions
    - `xxx_periph.h/*.c`  - includes all headers related to a peripheral; declaration and definition of IO mapping for that hardware