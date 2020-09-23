## `hal`

The `hal` component provides hardware abstraction and implementation for targets supported by ESP-IDF.

#### `include/hal`

`/include/hal` contains header files which provides a hardware-agnostic interface to the SoC. The interface consists of function declarations and abstracted types that other, higher level components can make use of in order to have code portable to all targets ESP-IDF supports.

It contains an abstraction layer for ineracting with/driving the hardware found in the SoC such as the peripherals and 'core' hardware such as the CPU, MPU, caches, etc. It contains for the abstracted types.
The abstraction design is actually two levels -- often somtimes `xxx_hal.h` includes a lower-level header from a `xxx_ll.h`,  which resides in the implementation. More on this abstraction design in the [`hal/include/hal`'s Readme](include/hal/readme.md)

#### `target/include`

Provides the implementation of the hardware-agnostic interface in the abstraction. Target-specific subdirectories exist for wildly different implementations among targets; while code that are common/very similar might be placed in the top-level of `/<target>/include`, using some amount of conditional preprocessors. It is up to the developers' discretion on which strategy to use. Code usually reside in source files with same names to header files whose interfaces they implement, ex. `xxx_hal.c` for `xxx_hal.h`.

As mentioned previously, the lower-level abstraction header `xxx_ll.h` resides in this directory, since they contain hardware-specific details.
However, what these can do is provide some abstraction among implementations, so that more code can be moved to the common, non-target-specific subdirectories.

This can also contain target-specific extensions to the HAL headers. These target-specific HAL headers have the same name and include the abstraction layer HAL header via `include_next`. These extensions might add more function declarations or override some things using macro magic.