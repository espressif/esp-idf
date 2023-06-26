# GPTimer Driver Design

## State Transition

> State transition is achieved by using the primitives provided by `<stdatomic.h>`.

```mermaid
stateDiagram-v2
    [*] --> init: gptimer_new_timer
    init --> enable: gptimer_enable
    enable --> init: gptimer_disable
    enable --> run: gptimer_start*
    run --> enable: gptimer_stop*
    init --> [*]: gptimer_del_timer
```

Other functions won't change the driver state. The functions above labeled with `*` are allowed to be used in the interrupt context.

## Concurrency

There might be race conditions when the user calls the APIs from a thread and interrupt at the same time. e.g. a Task is just running the `gptimer_start`, and suddenly an interrupt occurs, where the user calls `gptimer_stop` for the same timer handle. Which is possible to make a "stopped" timer continue to run if the interrupt is returned before the Task.

```mermaid
stateDiagram-v2
    state Race-Condition {
        Thread --> gptimer_start
        state gptimer_start {
            state is_enabled <<choice>>
            [*] --> is_enabled: Enabled?
            is_enabled --> run_wait: yes
            is_enabled --> [*] : no
            run_wait --> run: call HAL/LL functions to start timer
            }
        --
        Interrupt --> gptimer_stop
        state gptimer_stop {
            state is_running <<choice>>
            [*] --> is_running: Running?
            is_running --> enable_wait: yes
            is_running --> [*] : no
            enable_wait --> enable: call HAL/LL functions to stop timer
        }
    }
```

By introducing a "middle" state like `run_wait` and `enable_wait`, we make sure that the timer is in a safe state before we start/stop it. And if the state is invalid, it can return an error code to the user.
