| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP FSM and RISC-V Full Subproject Example

This example demonstrates two separate ULP full subprojects in one application. The main application starts a ULP FSM subproject first, then starts a ULP RISC-V subproject after the FSM counter completes.

The ULP FSM program is built from `main/ulp_fsm`, and the ULP RISC-V program is built from `main/ulp_riscv`. The parent app registers them independently with `ulp_add_project()`:

```cmake
ulp_add_project("ulp_fsm_main" "${CMAKE_CURRENT_LIST_DIR}/ulp_fsm/" TYPE fsm)
ulp_add_project("ulp_riscv_main" "${CMAKE_CURRENT_LIST_DIR}/ulp_riscv/" TYPE riscv)
```

At runtime, the FSM ULP increments `fsm_counter` to 5 and wakes the main CPU from light sleep. The main CPU then loads the RISC-V ULP, initializes `riscv_counter` from the FSM result, and lets the RISC-V ULP continue counting to 10.

## Example Output

```
ULP FSM and RISC-V full subproject example
Starting ULP FSM counter
ULP FSM counter: 5
Starting ULP RISC-V counter
ULP RISC-V counter: 10
Example finished
```
