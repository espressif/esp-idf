#include "sdkconfig.h"

#ifdef CONFIG_LEGACY_INCLUDE_COMMON_HEADERS
#warning Legacy including is enabled. This will be deprecated in the future. You can disable this option in the menuconfig. If there are some including errors, please try to include: "soc/soc.h", "soc/soc_memory_layout.h", "driver/gpio.h", or "esp_sleep.h".
#endif