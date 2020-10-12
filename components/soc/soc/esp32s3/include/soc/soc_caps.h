// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

#pragma once

#define SOC_TWAI_SUPPORTED 1
#define SOC_GDMA_SUPPORTED 1
#define SOC_CPU_CORES_NUM 2
#define SOC_CACHE_SUPPORT_WRAP    1

// Attention: These fixed DMA channels are temporarily workaround before we have a centralized DMA controller API to help alloc the channel dynamically
// Remove them when GDMA driver API is ready
#define SOC_GDMA_M2M_DMA_CHANNEL  (0)
#define SOC_GDMA_SPI2_DMA_CHANNEL (1)
#define SOC_GDMA_SPI3_DMA_CHANNEL (2)
#define SOC_GDMA_SHA_DMA_CHANNEL  (3)
