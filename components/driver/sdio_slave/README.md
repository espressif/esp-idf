# SDIO workflow

This README is a supplement to the comments in the SDIO slave driver. For more details, see `sdio_slave.c`.

The diagram are described in mermaid format. You can view the diagrams on Github or with any mermaid renders.

## Sending direction

```mermaid
sequenceDiagram
    actor task
    participant ISR
    participant HW
    Note right of HW: always: rx_done.raw = 1, rx_eof.ena= 1

    task-->>ISR: Prepare transfers
    task->>HW: rx_done.ena=1
    activate HW

    %% First ISR triggering transfer
    HW->>ISR: trigger
    activate ISR

    ISR->>HW: rx_done.ena=0
    deactivate HW

    loop every finished descriptor
    note over task, HW: skipped, see below
    end

    opt HW idle && new transfer prepared
    ISR->>HW: Load transfer
    end
    deactivate ISR

    activate HW
    HW->>HW: Host doing RX transfer
    deactivate HW

    HW->>HW: rx_eof.raw=1
    activate HW

    %% Second ISR recycling transfer
    HW->>ISR: trigger
    activate ISR

    ISR-->>HW: rx_done.ena=0

    loop every finished descriptor
    ISR->>HW: rx_eof.raw=0
    deactivate HW
    ISR-->>task: inform
    end

    opt HW idle && new transfer prepared
    note over ISR, HW: Load next transfer if there is...
    end

    deactivate ISR
```
