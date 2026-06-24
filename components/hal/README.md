# `hal` (G0 component)

⚠️ The HAL component is still under heavy development at the moment, so we don't guarantee the stability and backward-compatibility among versions.

The `hal` component provides a **Hardware Abstraction Layer** for all targets supported by ESP-IDF. It is designed to be a `G0` component so that it can be used by other components like `driver`, `esp_hw_support`, `esp_system` and so on.

In a broad sense, the HAL layer consists of two sub-layers: HAL (upper) and Low-Level(bottom). The HAL layer defines the steps and data that is required to operate a peripheral (e.g. initialization, start and stop). The low-level is a translation layer above the register files under the `soc` component, it only covers general conceptions to register configurations.

## Low-Level (`hal/<periph>_ll.h`)

Functions defined in the file must be static inlined. The first argument of an LL function is usually a pointer to the peripheral's base address [^1]. At the moment, each ESP target has its own set of Low-Level drivers. They're located under path e.g. `components/hal/<target>/include/hal/<periph>_ll.h`. We wish the the low-level functions could be as independent as possible, so that the caller doesn't need to worry about conflict between different sub-modules. For example, when resetting the driver of module A, the module B is also reset by accident. However, the digital design is not perfect, coupling happens from time to time.

### Handling Shared Registers

One of the biggest coupling is the so-called "hardware shared resource". Take the common `Reset and Clock Control` part as an example, the clock enable and disable logic of different peripherals are mixing in the same register. In RTOS environment, it's super easy to make a mistake when you enable peripheral A and then peripheral B is disabled by accident. A simple way to avoid such mistake is to using a critical section when accessing such shared registers. However from the point of the software architecture, it's not a good idea to add a **lock** in the Low-Level because it's a concept of the operating system.

One compromise is to **highlight** the LL function which needs the caller to use them in a critical section. e.g.

```c
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timer_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        timer_ll_reset_register(__VA_ARGS__); \
    } while(0)

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define timer_ll_set_clock_source(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        timer_ll_set_clock_source(__VA_ARGS__); \
    } while(0)
```

By referencing a variable which is only declared in the critical section, the compiler will report an error if the caller forgets to use the critical section. The following macros are provided by `esp_private/periph_ctrl.h`, which contain the above *magic* variables.

| Macro | Private variables used to declare a critical section | Use condition |
| --- | --- | --- |
| `PERIPH_RCC_ACQUIRE_ATOMIC` | *__DECLARE_RCC_RC_ATOMIC_ENV* | This critical section not only protects the shared register accessing, but also **increases** a reference counter of the peripheral module. </br> You should use this critical section if the peripheral module has multiple independent sub-modules. |
| `PERIPH_RCC_RELEASE_ATOMIC` | *__DECLARE_RCC_RC_ATOMIC_ENV* | This critical section not only protects the shared register accessing, but also **decreases** a reference counter of the peripheral module. </br> You should use this critical section if the peripheral module has multiple independent sub-modules. |
| `PERIPH_RCC_ATOMIC` | *__DECLARE_RCC_ATOMIC_ENV* | This critical section only protects the shared register accessing. |

ESP-IDF driver developers then can use the above macros to call the special LL functions. e.g.

```c
static void enable_timer_group0_for_calibration(void)
{
    PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_TIMG0_MODULE, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(0, true);
            timer_ll_reset_register(0);
        }
    }
}
```

## HAL (`hal/<periph>_hal.h`)

This layer is a combination of Low-Level function calls, aiming to ease the load when porting a new chip to other platforms (e.g. Zephyr). This layer shouldn't rely on Operating System, i.e., don't use primitives that only offered by an Operating System, e.g., the lock and other blocking functions. Please don't introduce any *driver models* in the HAL layer so that the non-idf developers can customized their own drivers according to their platform requirement.

The first argument of a HAL function is usually a pointer to the **context object**. The context object is a structure which saves the necessary information that is used by the HAL driver (e.g. the base address of the peripheral). ⚡ Please note, the memory used by the HAL context object is allocated by the caller, so the HAL driver shouldn't free it.

## File Structure

### `include/hal`

`/include/hal` contains header files which provides a hardware-agnostic interface to the SoC. The interface consists of function declarations and abstracted types that other, higher level components can make use of in order to have code portable to all targets ESP-IDF supports.

It contains an abstraction layer for interacting with/driving the hardware found in the SoC such as the peripherals and 'core' hardware such as the CPU, MPU, caches, etc. It contains for the abstracted types.
The abstraction design is actually two levels -- often sometimes `xxx_hal.h` includes a lower-level header from a `xxx_ll.h`,  which resides in the implementation.

### `target/include`

Provides the implementation of the hardware-agnostic interface in the abstraction. Target-specific subdirectories exist for wildly different implementations among targets; while code that are common/very similar might be placed in the top-level of `/<target>/include`, using some amount of conditional preprocessor. It is up to the developers' discretion on which strategy to use. Code usually reside in source files with same names to header files whose interfaces they implement, ex. `xxx_hal.c` for `xxx_hal.h`.

As mentioned previously, the lower-level abstraction header `xxx_ll.h` resides in this directory, since they contain hardware-specific details.
However, what these can do is provide some abstraction among implementations, so that more code can be moved to the common, non-target-specific subdirectories.

This can also contain target-specific extensions to the HAL headers. These target-specific HAL headers have the same name and include the abstraction layer HAL header via `include_next`. These extensions might add more function declarations or override some things using macro magic.

[^1]: This is not a must. Sometimes if the LL is just operating some system level registers, you don't have to provide this argument.
