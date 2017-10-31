#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "esp_pm.h"


TEST_CASE("Can dump power management lock stats", "[pm]")
{
    esp_pm_dump_locks(stdout);
}
