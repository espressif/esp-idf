# PCNT Driver Design

## Concurrency

The count value and the overflow state of the count value are located in *different* registers, resulting in the software being unable to obtain information from both of them in the same read instruction.

The race condition case is as follow:

```mermaid
sequenceDiagram
    participant HW as PCNT Hardware
    participant CPU0_ISR as CPU0_ISR
    participant CPU1_Task as CPU1_Task (pcnt_unit_get_count)
    participant REG as Reg and Soft accum counter State

    CPU1_Task->>CPU1_Task: Call pcnt_unit_get_count()
    Note over REG: intr_status = 0<br/>cnt_reg = cnt_value<br/>accum_value = old_value
    CPU1_Task->>CPU1_Task: portENTER_CRITICAL_SAFE()
    CPU1_Task->>REG: Read intr_status
    Note over CPU1_Task: intr_status=0, no need to do compensation
    HW->>REG: Overflow interrupt triggered
    Note over REG: intr_status = 1<br/>cnt_reg = 0<br/>accum_value = old_value
    REG->>CPU0_ISR: ISR is called
    CPU0_ISR->>CPU0_ISR: try portENTER_CRITICAL_SAFE() but spin

    CPU1_Task->>REG: Read cnt_reg(0) + accum_value(old)
    CPU1_Task->>CPU1_Task: portEXIT_CRITICAL_SAFE()

    CPU0_ISR->>CPU0_ISR: portENTER_CRITICAL_SAFE()
    CPU0_ISR->>REG: Clear interrupt status and update accum_value
    Note over REG: intr_status = 0<br/>accum_value = new_value
    CPU0_ISR->>CPU0_ISR: portEXIT_CRITICAL_SAFE()
    
    Note over CPU0_ISR: Process events
    Note over CPU1_Task: Return incorrect count ❌

```

In the software, we determine whether to perform compensation by checking whether the count value exceeds half of the limit. This can prevent counting errors when the overflow frequency is not high.
