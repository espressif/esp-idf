/*
 Tests for the Wi-Fi
*/
#include "unity.h"
#include "esp_log.h"
#include "esp_wifi_internal.h"

TEST_CASE("wifi lib git commit id","[wifi]")
{    
    TEST_ESP_OK( esp_wifi_internal_git_commit_id_check() );
}

