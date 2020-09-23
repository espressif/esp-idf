## `soc` ##

The `soc` component provides provides hardware description and hardware support functionality for targets supported by ESP-IDF. This is reflected in the component's subdirectories:

- `soc/include` - abstraction
- `soc/soc` - description
- `soc/src` - implementation

#### `soc/include` ####

`soc/include` contains header files which provide a hardware-agnostic interface to the SoC. The interface consists of
function declarations and abstracted types that other, higher level components can make use of in order to have code portable to
all targets ESP-IDF supports.

The `soc` subdirectory contains other useful interface for SoC-level operations or concepts, such as the memory layout, spinlocks, etc.

#### `soc/soc` ####

The `soc/soc` subdirectory contains description of the underlying hardware:

    - `xxx_reg.h`   - defines registers related to the hardware
    - `xxx_struct.h` - hardware description in C `struct`
    - `xxx_channel.h` - definitions for hardware with multiple channels
    - `xxx_caps.h`  - features/capabilities of the hardware
    - `xxx_pins.h`  - pin definitions
    - `xxx_periph.h/*.c`  - includes all headers related to a peripheral; declaration and definition of IO mapping for that hardware

Since the hardware description is target-specific, there are subdirectories for each target containing copies of the files above.
Furthermore, the files in this directory should be standalone, i.e. should not include files from outside directories.

#### `soc/src` ####

Provides the interface about the memory, I2C, and Real_Time Clock functions.