#include <stdio.h>
#include "esp_log.h"

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);
}

uint32_t esp_log_timestamp()
{
    return 0;
}
