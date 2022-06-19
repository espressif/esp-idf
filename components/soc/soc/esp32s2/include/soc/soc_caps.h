// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

#pragma once

#define SOC_TWAI_SUPPORTED 1
#define SOC_CPU_CORES_NUM 1
#define SOC_SUPPORTS_SECURE_DL_MODE 1

#define SOC_CACHE_SUPPORT_WRAP    1

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

