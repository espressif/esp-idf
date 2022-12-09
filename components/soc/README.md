## `soc` ##

The `soc` component provides abstraction, hardware description and implementation for targets suppported by ESP-IDF. This is reflected in
the component's subdirectories:

- `soc/include` - abstraction
- `soc/soc` - description
- `soc/src` - implementation

#### `soc/include` ####

`soc/include` contains header files which provide a hardware-agnostic interface to the SoC. The interface consists of
function declarations and abstracted types that other, higher level components can make use of in order to have code portable to 
all targets ESP-IDF supports.

The `hal` subdirectory contains an abstraction layer for interacting with/driving the hardware found in the SoC such as the peripherals 
and 'core' hardware such as the CPU, MPU, caches, etc. It contains `xxx_hal.h` files for the function declarations and `xxx_types.h` for the abstracted types.
The abstraction design is actually two levels -- oftentimes `xxx_hal.h` includes a lower-level header from a
`xxx_ll.h`, which resides in the implementation, `soc/src` subdirectory. More on this abstraction design in the [`hal` subdirectory's README](include/hal/readme.md).

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

Provides the implementation of the hardware-agnostic interface in the abstraction. Target-specific subdirectories exist for wildly different implementations between targets; while code that are common/very similar might be placed in the top-level of `soc/src`, using some amount of conditional preprocessors. It is up to the developers' discretion on which strategy to use. Code usually reside in source files with same names to header files whose interfaces they implement, ex. `xxx_hal.c` for `xxx_hal.h`.

As mentioned previously, the lower-level abstraction header `xxx_ll.h` resides in this directory, since they contain hardware-specific details.
However, what these can do is provide some abstraction among implementations, so that more code can be moved to the common, non-target-specific subdirectories.

This can also contain target-specific extensions to the HAL headers. These target-specific HAL headers have the same name and include the abstraction layer HAL header via `include_next`. These extensions might add more function declarations or override some things using macro magic.