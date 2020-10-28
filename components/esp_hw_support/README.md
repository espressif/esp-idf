## `esp_hw_support` ##

This component contains hardware-related operations for supporting the system. These operations
are one level above that of `hal` in that these(1) use system services such as memory allocation, logging, scheduling
or (2) may be multi-step operations involving/affecting multiple parts of the SoC.

Implementations that don't fit other components cleanly, but are not worth creating a new component for (yet) may also be placed here as long as they don't pull dependencies other than the core system components.