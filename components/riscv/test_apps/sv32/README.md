| Supported Targets | ESP32-S31 |
| ----------------- | --------- |

# RISC-V Sv32 tests

This focused ESP32-S31 Unity application checks the trusted RISC-V Sv32
primitives: entry and `satp` encodings, Sv32/Bare CSR transitions, local
invalidation, and one translated load/store path. Page-fault, permission,
mapping-policy and multicore coverage belongs in the upper layers that consume
these architectural helpers.

The translated accesses stay in M-mode and use `mstatus.MPRV`/`MPP` so that only
one inline-assembly load or store is subject to Sv32 translation as an S-mode
access. No trap handler is installed; an unexpected fault panics and fails the
test.

The fixture deliberately places its page tables and backing pages in aligned HP
SRAM. Production callers remain responsible for selecting walker-visible memory,
establishing any required PMP/PMA policy, and coordinating active roots and TLB
invalidation across cores. This suite does not exercise remote shootdown.
