#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32_le(uint32_t crc, const uint8_t* buf, size_t len);

#ifdef __cplusplus
}
#endif

