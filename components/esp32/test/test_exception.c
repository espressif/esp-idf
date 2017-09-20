#include "unity.h"
#include "esp_system.h"
#include "string.h"


TEST_CASE("make exception", "[restart][reset=StoreProhibited,SW_CPU_RESET]")
{
    *(int *) NULL = 0;
}
