#include <unistd.h>
#include "freertos/task.h"

void vTaskDelay( const uint32_t xTicksToDelay )
{
    sleep(xTicksToDelay);
}