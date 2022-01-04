// Note: these undefs and defines are used to suppress warnings and errors when compiling esp32 idf on host gcc/clang
#undef  __nonnull
#define __warning__ deprecated
#define IRAM_ATTR
#define __ESP_ATTR_H__
#include <features.h>
