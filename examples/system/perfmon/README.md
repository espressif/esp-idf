| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# Performance Monitor (`perfmon`) example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview
This example illustrates usage of `perfmon` APIs to monitor and profile functions.
The example will calculate performance statistic for simple test function.
The simple test function could be exchanged to one from the user.

The example contain test function that will be executed with perfmon component and collect CPU statistic. The test function will be executed 200 times in each test case. The first test case collect statistic from all available performance counters, and second test just from defined in the list.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32, ESP32-S2 or ESP32-S3 development board. `perfmon` component isn't supported on Espressif chips with RISC-V CPU architecture.

### Build and Flash

```
idf.py build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

1. Example starts and calls the first test. This test calls the test function `exec_test_function` 200 times and collects all CPU performance metrics.

    <details><summary>Example log output:</summary>

    ```
    I (288) example: Start
    I (288) example: Start test with printing all available statistic
    Value =       750, select =  0, mask = 0001.  Counts cycles.
                      Amount of cycles
    Value =         0, select =  1, mask = 0001.  Overflow of counter.
                      Overflow counter
    Value =         0, select =  2, mask = 0001.  Successfully Retired Instructions.
    
                      JX instructions
    Value =         0, select =  2, mask = 0002.  Successfully Retired Instructions.
    
                      CALLXn instructions
    Value =         3, select =  2, mask = 0004.  Successfully Retired Instructions.
    
                      return instructions (RET, RETW, ...)
    Value =         0, select =  2, mask = 0008.  Successfully Retired Instructions.
    
                      supervisor return instructions (RFDE, RFE, RFI, RFWO, RFWU)
    Value =       100, select =  2, mask = 0010.  Successfully Retired Instructions.
    
                      Conditional branch instructions where execution
                      transfers to the target (aka. taken branch),
                       or loopgtz/loopnez instr where execution skips
                       the loop (aka. not-taken loop)
    Value =         0, select =  2, mask = 0020.  Successfully Retired Instructions.
    
                      J instr
    Value =         1, select =  2, mask = 0040.  Successfully Retired Instructions.
    
                      CALLn instr
    Value =         0, select =  2, mask = 0080.  Successfully Retired Instructions.
    
                      Conditional branch instr where execution
                       falls through (aka. not-taken branch)
    Value =         0, select =  2, mask = 0100.  Successfully Retired Instructions.
    
                      Loop instr where execution falls into loop (aka. taken loop)
    Value =         0, select =  2, mask = 0400.  Successfully Retired Instructions.
    
                      Last inst of loop and execution transfers
                       to LBEG (aka. loopback taken)
    Value =         0, select =  2, mask = 0800.  Successfully Retired Instructions.
    
                      Last inst of loop and execution falls
                       through to LEND (aka. loopback fallthrough)
    Value =       309, select =  2, mask = 8000.  Successfully Retired Instructions.
    
                      Non-branch instr (aka. non-CTI)
    Value =         0, select =  3, mask = 0002.  Data-related GlobalStall cycles.
                      Store buffer full stall
    Value =         0, select =  3, mask = 0004.  Data-related GlobalStall cycles.
                      Store buffer conflict stall
    Value =         0, select =  3, mask = 0008.  Data-related GlobalStall cycles.
                      Data Cache-miss stall (unused)
    Value =         0, select =  3, mask = 0010.  Data-related GlobalStall cycles.
                      Data RAM/ROM/XLMI busy stall
    Value =         0, select =  3, mask = 0020.  Data-related GlobalStall cycles.
                      Data inbound-PIF request stall (includes s32c1i)
    Value =         0, select =  3, mask = 0040.  Data-related GlobalStall cycles.
                      MHT lookup stall
    Value =         0, select =  3, mask = 0080.  Data-related GlobalStall cycles.
                      Uncached load stall (included in MHT lookup stall below)
    Value =         0, select =  3, mask = 0100.  Data-related GlobalStall cycles.
                      Bank-conflict stall
    Value =         0, select =  4, mask = 0001.  Instruction-related and Other Glob
    alStall cycles.
                      ICache-miss stall
    Value =         0, select =  4, mask = 0002.  Instruction-related and Other Glob
    alStall cycles.
                      Instruction RAM/ROM busy stall
    Value =         0, select =  4, mask = 0004.  Instruction-related and Other Glob
    alStall cycles.
                      Instruction RAM inbound-PIF request stall
    Value =         0, select =  4, mask = 0008.  Instruction-related and Other Glob
    alStall cycles.
                      TIE port stall
    Value =         0, select =  4, mask = 0010.  Instruction-related and Other Glob
    alStall cycles.
                      External RunStall signal status
    Value =         0, select =  4, mask = 0020.  Instruction-related and Other Glob
    alStall cycles.
                      Uncached fetch stall
    Value =         1, select =  4, mask = 0040.  Instruction-related and Other Glob
    alStall cycles.
                      FastL32R stall
    Value =         0, select =  4, mask = 0080.  Instruction-related and Other Glob
    alStall cycles.
                      Iterative multiply stall
    Value =         0, select =  4, mask = 0100.  Instruction-related and Other Glob
    alStall cycles.
                      Iterative divide stall
    Value =         0, select =  5, mask = 0001.  Exceptions and Pipeline Replays.
                      Other Pipeline Replay (i.e. excludes cache miss etc.)
    Value =         0, select =  5, mask = 0002.  Exceptions and Pipeline Replays.
                      Level-1 interrupt
    Value =         0, select =  5, mask = 0004.  Exceptions and Pipeline Replays.
                      Greater-than-level-1 interrupt
    Value =         0, select =  5, mask = 0008.  Exceptions and Pipeline Replays.
                      Debug exception
    Value =         0, select =  5, mask = 0010.  Exceptions and Pipeline Replays.
                      NMI
    Value =         0, select =  5, mask = 0020.  Exceptions and Pipeline Replays.
                      Window exception
    Value =         0, select =  5, mask = 0040.  Exceptions and Pipeline Replays.
                      Allocate exception
    Value =         0, select =  5, mask = 0080.  Exceptions and Pipeline Replays.
                      Other exceptions
    Value =         0, select =  5, mask = 0100.  Exceptions and Pipeline Replays.
                      HW-corrected memory error
    Value =         0, select =  6, mask = 0001.  Hold and Other Bubble cycles.
                      Processor domain PSO bubble
    Value =         0, select =  6, mask = 0004.  Hold and Other Bubble cycles.
                      R hold caused by Data Cache miss(unused)
    Value =         0, select =  6, mask = 0008.  Hold and Other Bubble cycles.
                      R hold caused by Store release
    Value =         0, select =  6, mask = 0010.  Hold and Other Bubble cycles.
                      R hold caused by register dependency
    Value =         0, select =  6, mask = 0020.  Hold and Other Bubble cycles.
                      R hold caused by MEMW, EXTW or EXCW
    Value =         0, select =  6, mask = 0040.  Hold and Other Bubble cycles.
                      R hold caused by Halt instruction (TX only)
    Value =       322, select =  6, mask = 0080.  Hold and Other Bubble cycles.
                      CTI bubble (e.g. branch delay slot)
    Value =         0, select =  6, mask = 0100.  Hold and Other Bubble cycles.
                      WAITI bubble i.e. a cycle spent in WaitI power down mode.
    Value =       417, select =  7, mask = 0001.  Instruction TLB Accesses (per inst
    ruction retiring).
                      ITLB Hit
    Value =         0, select =  7, mask = 0002.  Instruction TLB Accesses (per inst
    ruction retiring).
                      Replay of instruction due to ITLB miss
    Value =         0, select =  7, mask = 0004.  Instruction TLB Accesses (per inst
    ruction retiring).
                      HW-assisted TLB Refill completes
    Value =         0, select =  7, mask = 0008.  Instruction TLB Accesses (per inst
    ruction retiring).
                      ITLB Miss Exception
    Value =         0, select =  8, mask = 0001.  Instruction Memory Accesses (per i
    nstruction retiring).
                      Instruction Cache Hit
    Value =         0, select =  8, mask = 0002.  Instruction Memory Accesses (per i
    nstruction retiring).
                      Instruction Cache Miss
    Value =       420, select =  8, mask = 0004.  Instruction Memory Accesses (per i
    nstruction retiring).
                      All InstRAM or InstROM accesses
    Value =         0, select =  8, mask = 0008.  Instruction Memory Accesses (per i
    nstruction retiring).
                      Bypass (i.e. uncached) fetch
    Value =         3, select =  9, mask = 0001.  Data TLB Accesses.
                      DTLB Hit
    Value =         0, select =  9, mask = 0002.  Data TLB Accesses.
                      Replay of load/store due to DTLB miss
    Value =         0, select =  9, mask = 0004.  Data TLB Accesses.
                      HW-assisted TLB Refill completes
    Value =         0, select =  9, mask = 0008.  Data TLB Accesses.
                      DTLB Miss Exception
    Value =         0, select = 10, mask = 0001.  Load Instruction (Data Memory).
                      Data Cache Hit(unused)
    Value =         0, select = 10, mask = 0002.  Load Instruction (Data Memory).
                      Data Cache Miss(unused)
    Value =         3, select = 10, mask = 0004.  Load Instruction (Data Memory).
                      Load from local memory i.e. DataRAM, DataROM, InstRAM, InstROM
    
    Value =         0, select = 10, mask = 0008.  Load Instruction (Data Memory).
                      Bypass (i.e. uncached) load
    Value =         0, select = 13, mask = 0001.  Load Instruction (Data Memory).
                      Data Cache Hit(unused)
    Value =         0, select = 13, mask = 0002.  Load Instruction (Data Memory).
                      Data Cache Miss(unused)
    Value =         0, select = 13, mask = 0004.  Load Instruction (Data Memory).
                      Load from local memory i.e. DataRAM, DataROM, InstRAM, InstROM
    
    Value =         0, select = 13, mask = 0008.  Load Instruction (Data Memory).
                      Bypass (i.e. uncached) load
    Value =         0, select = 16, mask = 0001.  Load Instruction (Data Memory).
                      Data Cache Hit (unused)
    Value =         0, select = 16, mask = 0002.  Load Instruction (Data Memory).
                      Data Cache Miss (unused)
    Value =         0, select = 16, mask = 0004.  Load Instruction (Data Memory).
                      Load from local memory i.e. DataRAM, DataROM, InstRAM, InstROM
    
    Value =         0, select = 16, mask = 0008.  Load Instruction (Data Memory).
                      Bypass (i.e. uncached) load
    Value =         0, select = 11, mask = 0001.  Store Instruction (Data Memory).
                      Data Cache Hit (unused)
    Value =         0, select = 11, mask = 0002.  Store Instruction (Data Memory).
                      Data Cache Miss (unused)
    Value =         0, select = 11, mask = 0004.  Store Instruction (Data Memory).
                      Store to local memory i.e. DataRAM, InstRAM
    Value =         0, select = 11, mask = 0008.  Store Instruction (Data Memory).
                      PIF Store
    Value =         0, select = 14, mask = 0001.  Store Instruction (Data Memory).
                      Data Cache Hit(unused)
    Value =         0, select = 14, mask = 0002.  Store Instruction (Data Memory).
                      Data Cache Miss(unused)
    Value =         0, select = 14, mask = 0004.  Store Instruction (Data Memory).
                      Store to local memory i.e. DataRAM, InstRAM
    Value =         0, select = 14, mask = 0008.  Store Instruction (Data Memory).
                      PIF Store
    Value =         0, select = 17, mask = 0001.  Store Instruction (Data Memory).
                      Data Cache Hit (unused)
    Value =         0, select = 17, mask = 0002.  Store Instruction (Data Memory).
                      Data Cache Miss (unused)
    Value =         0, select = 17, mask = 0004.  Store Instruction (Data Memory).
                      Store to local memory i.e. DataRAM, InstRAM
    Value =         0, select = 17, mask = 0008.  Store Instruction (Data Memory).
                      PIF Store
    Value =         0, select = 12, mask = 0001.  Accesses to Data Memory (Load, Sto
    re, S32C1I, ...).
                      Cache Miss
    Value =         0, select = 15, mask = 0001.  Accesses to Data Memory (Load, Sto
    re, S32C1I, ...).
                      Cache Miss
    Value =         0, select = 18, mask = 0001.  Accesses to Data Memory (Load, Sto
    re, S32C1I, ...).
                      Cache Miss
    Value =       415, select = 22, mask = 0001.  Multiple Load/Store.
                      0 stores and 0 loads
    Value =         3, select = 22, mask = 0002.  Multiple Load/Store.
                      0 stores and 1 loads
    Value =         0, select = 22, mask = 0004.  Multiple Load/Store.
                      1 stores and 0 loads
    Value =         0, select = 22, mask = 0008.  Multiple Load/Store.
                      1 stores and 1 loads
    Value =         0, select = 22, mask = 0010.  Multiple Load/Store.
                      0 stores and 2 loads
    Value =         0, select = 22, mask = 0020.  Multiple Load/Store.
                      2 stores and 0 loads
    Value =         0, select = 23, mask = 0001.  Outbound PIF.
                      Castout
    Value =         0, select = 23, mask = 0002.  Outbound PIF.
                      Prefetch
    Value =         0, select = 24, mask = 0001.  Inbound PIF.
                      Data DMA
    Value =         0, select = 24, mask = 0002.  Inbound PIF.
                      Instruction DMA
    Value =         0, select = 26, mask = 0001.  Prefetch.
                      I prefetch-buffer-lookup hit
    Value =         0, select = 26, mask = 0002.  Prefetch.
                      D prefetch-buffer-lookup hit
    Value =         0, select = 26, mask = 0004.  Prefetch.
                      I prefetch-buffer-lookup miss
    Value =         0, select = 26, mask = 0008.  Prefetch.
                      D prefetch-buffer-lookup miss
    Value =         0, select = 26, mask = 0020.  Prefetch.
                      Direct fill to (L1) Data Cache (unused)
    Value =         0, select = 27, mask = 0001.  iDMA.
                      active cycles
    Value =         0, select = 28, mask = 0001.  Length of Instructions.
                      16-bit
    Value =         0, select = 28, mask = 0002.  Length of Instructions.
                      24-bit
    Value =         0, select = 28, mask = 0004.  Length of Instructions.
                      32-bit
    Value =         0, select = 28, mask = 0008.  Length of Instructions.
                      40-bit
    Value =         0, select = 28, mask = 0010.  Length of Instructions.
                      48-bit
    Value =         0, select = 28, mask = 0020.  Length of Instructions.
                      56-bit
    Value =         0, select = 28, mask = 0040.  Length of Instructions.
                      64-bit
    Value =         0, select = 28, mask = 0080.  Length of Instructions.
                      72-bit
    Value =         0, select = 28, mask = 0100.  Length of Instructions.
                      80-bit
    Value =         0, select = 28, mask = 0200.  Length of Instructions.
                      88-bit
    Value =         0, select = 28, mask = 0400.  Length of Instructions.
                      96-bit
    Value =         0, select = 28, mask = 0800.  Length of Instructions.
                      104-bit
    Value =         0, select = 28, mask = 1000.  Length of Instructions.
                      112-bit
    Value =         0, select = 28, mask = 2000.  Length of Instructions.
                      120-bit
    Value =         0, select = 28, mask = 4000.  Length of Instructions.
                      128-bit
    ```
    </details>

2. Example calls the second test, which runs the same function under tests. This time, a user-provided list of metrics is measured.

    <details><summary>Example log output:</summary>

    ```
    I (1588) example: Start test with user defined statistic
    Value =       743, select =  0, mask = 0001.  Counts cycles.
                      Amount of cycles
    Value =       417, select =  2, mask = 8dff.  Successfully Retired Instructions.
    
                      JX instructions
                      CALLXn instructions
                      return instructions (RET, RETW, ...)
                      supervisor return instructions (RFDE, RFE, RFI, RFWO, RFWU)
                      Conditional branch instructions where execution
                      transfers to the target (aka. taken branch),
                       or loopgtz/loopnez instr where execution skips
                       the loop (aka. not-taken loop)
                      J instr
                      CALLn instr
                      Conditional branch instr where execution
                       falls through (aka. not-taken branch)
                      Loop instr where execution falls into loop (aka. taken loop)
                      Last inst of loop and execution transfers
                       to LBEG (aka. loopback taken)
                      Last inst of loop and execution falls
                       through to LEND (aka. loopback fallthrough)
                      Non-branch instr (aka. non-CTI)
    Value =         3, select = 10, mask = 0004.  Load Instruction (Data Memory).
                      Load from local memory i.e. DataRAM, DataROM, InstRAM, InstROM
    
    Value =         0, select = 11, mask = 0004.  Store Instruction (Data Memory).
                      Store to local memory i.e. DataRAM, InstRAM
    Value =       321, select =  6, mask = 01ed.  Hold and Other Bubble cycles.
                      Processor domain PSO bubble
                      R hold caused by Data Cache miss(unused)
                      R hold caused by Store release
                      R hold caused by MEMW, EXTW or EXCW
                      R hold caused by Halt instruction (TX only)
                      CTI bubble (e.g. branch delay slot)
                      WAITI bubble i.e. a cycle spent in WaitI power down mode.
    Value =         0, select =  6, mask = 0010.  Hold and Other Bubble cycles.
                      R hold caused by register dependency
    Value =         0, select =  1, mask = 0001.  Overflow of counter.
                      Overflow counter
    I (1788) example: The End
    
    ```
    </details>
