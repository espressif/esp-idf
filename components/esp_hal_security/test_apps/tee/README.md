| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# TEE (Trusted Execution Environment) Test Application

This application is designed to validate the key components of the **ESP-TEE** framework, with a focus on:

1. **Permission Management (PMS: TEE controller + APM module)**
   - Region-based memory and peripheral access control using the TEE controller and the Access Permission Management (APM) module

2. **Interrupts**
   - Interrupt handling in Machine mode (M), User mode (U), and cross-mode contexts

It is primarily intended for early bring-up and SoC-level functional validation.

---

## Test Coverage: PMS (TEE + APM module)

### TEE mode default access behavior

- **TEE Mode Default Access**

Validates whether TEE mode has unrestricted access to regions not covered by APM entries. Confirms filtering issues on ESP32-C6 and ESP32-H2.

### HP_CPU Access

- **HP_CPU → CPU_PERI**
- **HP_CPU → HP_PERI**
- **HP_CPU → LP_PERI**
- **HP_CPU → HP_MEM** (if `SOC_APM_CPU_APM_SUPPORTED`)
- **HP_CPU → LP_MEM** (if `SOC_RTC_MEM_SUPPORTED`)

Each scenario checks access permission enforcement for read-only and write-only settings across security modes.

### GDMA Access

- **GDMA → HP_MEM**
- **GDMA → EXT_MEM** (if `CONFIG_SPIRAM`)

Validates region-based restrictions across modes using DMA transfers.

### LP_CPU Access (if `CONFIG_ULP_COPROC_ENABLED`)

- **LP_CPU → LP_PERI**
- **LP_CPU → LP_MEM**
- **LP_CPU → HP_MEM**

### PERI_APM Tests (if `SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL`)

- **HP_CPU → HP_PERI**
- **HP_CPU → LP_PERI**
- **LP_CPU → HP_PERI**
- **LP_CPU → LP_PERI**

Validates the per-peripheral access permissions for all security modes.

---

## Test Coverage: Interrupts

- **M-mode interrupts in M-mode**
- **U-mode interrupts in U-mode**
- **M-mode interrupts in U-mode**
- **U-mode interrupts in M-mode**

## Target Extension Guide

To add support for a new SoC target, create a test configuration header at:

```
components/pms/priv_include/<target>/test_pms_params.h
```

This header must define:

### 1. GDMA APM Master ID

Defined based on GDMA version:

```c
#define TEST_GDMA_APM_MASTER_ID APM_MASTER_GDMA_GPSPI  // For SOC_AHB_GDMA_VERSION == 1
```

- `SOC_AHB_GDMA_VERSION == 1` → `GPSPI`
- `SOC_AHB_GDMA_VERSION == 2` → `26` (e.g., `GDMA_DUMMY10`)

Refer to `esp_hal_security/apm_types.h` or the SoC TRM.

### 2. APM Controller and Path Definitions

Specify APM controller and access path for each initiator-target pair:

```c
#define HP_CPU_CPUPERI_APM_CTRL APM_CTRL_HP_APM
#define HP_CPU_CPUPERI_APM_PATH APM_CTRL_ACCESS_PATH_M0
#define TEST_HP_CPU_CPUPERI_REGN_NUM 4
```

Use the TRM to determine path-controller mappings.

### 3. Peripheral Test Region Definitions

Split the peripheral address space into testable regions:

- Use `soc/reg_base.h` to get base addresses of peripherals.
- Align non-contiguous region boundaries to 4 KB using:

```c
#define ALIGN_TO_NEXT_4KB(addr) (((addr) + 0x1000) & ~0xFFF)
```

#### CPU_PERI

- Typically monitored by `HP_APM`.
- Includes blocks like `TRACE`, `ASSIST_DEBUG`, `INTPRI`, `CACHE`.
- `CPU_PERI` typically includes ~4 peripherals, and thus, 4 APM regions are sufficient for test coverage.

#### LP_PERI

- Monitored by `LP_APM`.
- Covers domains like `PMU`, `LP_IO`, `LP_AON`.
- For SoCs with `LP_CPU`, reserve the regions containing `PMU`, `LP_AON`, `LP_PERI`. These are reserved to allow test-case control and avoid undesired APM violations.

```c
#define TEST_LP_PERI_RESV_MASK BIT(0) | BIT(2) | BIT(6)
```

#### HP_PERI

- Monitored by `HP_APM`.
- Split into: `HP_PERI0`, `HP_PERI1`, and `HP_PERI2`.
  Test APM regions should be proportionally allocated to these segments based on address space size.
- Reserve the region containing `UART0`, since it is often used by the log console.
- If `PERI_APM` is supported, reserve the regions with `TEE`, `LP_TEE`, `HP_APM`, `LP_APM`, `LP_APM0`, and `CPU_APM`, as these are inaccessible to the REE modes.

```c
#define TEST_HP_PERI_RESV_MASK BIT(0)
```

---

## Building

```bash
idf.py set-target <TARGET>
idf.py build
```

## Running the App

```bash
idf.py flash monitor
```

## Running Tests

```bash
pytest --target <TARGET>
```

---
