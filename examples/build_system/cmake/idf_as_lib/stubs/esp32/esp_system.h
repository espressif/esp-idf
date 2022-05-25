#pragma once
#include <stdint.h>

#define CHIP_FEATURE_EMB_FLASH  (1UL << 0)
#define CHIP_FEATURE_BT         (1UL << 4)
#define CHIP_FEATURE_BLE        (1UL << 5)

typedef struct {
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint16_t revision;       //!< chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
    uint8_t cores;           //!< number of CPU cores
} esp_chip_info_t;

void esp_restart(void);
void esp_chip_info(esp_chip_info_t* out_info);
