#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stddef.h>

#include "esp_system.h"

uint32_t esp_random(void)
{
    return (uint32_t)rand();
}

