#include "test_utils.h"
#include "mdns.h"
#include "unity.h"


#define MDNS_HOSTNAME "test-hostname"
#define MDNS_INSTANCE "test-instance"
#define MDNS_SERVICE_NAME  "_http"
#define MDNS_SERVICE_PROTO "_tcp"
#define MDNS_SERVICE_PORT   80


static void yield_to_all_priorities(void)
{
    // Lower the test-task priority before testing to ensure other tasks got executed on forced context switch
    size_t test_task_prio_before = uxTaskPriorityGet(NULL);
    vTaskPrioritySet(NULL, tskIDLE_PRIORITY);
    taskYIELD(); // Let the RTOS to switch context
    vTaskPrioritySet(NULL, test_task_prio_before);
}


TEST_CASE("mdns api to fail in invalid state", "[mdns][leaks=64]")
{
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_init() );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_hostname_set(MDNS_HOSTNAME) );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_instance_name_set(MDNS_INSTANCE) );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0) );
}

TEST_CASE("mdns init and deinit", "[mdns][leaks=64]")
{
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    yield_to_all_priorities(); // Make sure that mdns task has executed to complete initialization
    mdns_free();
    esp_event_loop_delete_default();
}

TEST_CASE("mdns api return expected err-code and do not leak memory", "[mdns][leaks=64]")
{
    mdns_txt_item_t serviceTxtData[CONFIG_MDNS_MAX_SERVICES] = { {NULL, NULL},
    };
    for (int i=0; i<CONFIG_MDNS_MAX_SERVICES; ++i) {
        serviceTxtData[i].key = "Key";
        serviceTxtData[i].value = "Value";
    }
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());

    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_hostname_set(MDNS_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_instance_name_set(MDNS_INSTANCE) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, serviceTxtData, CONFIG_MDNS_MAX_SERVICES) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, serviceTxtData, CONFIG_MDNS_MAX_SERVICES) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_item_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, "key1", "value1") );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_item_remove(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, "key1") );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_port_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 8080) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_remove(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO) );
    yield_to_all_priorities();  // Make sure that mdns task has executed to remove the service

    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_remove_all() );
    yield_to_all_priorities();  // Make sure that mdns task has executed to remove all services

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, mdns_service_port_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 8080) );

    mdns_free();
    esp_event_loop_delete_default();
}

TEST_CASE("mdns query api return expected err-code and do not leak memory", "[leaks=64]")
{
    mdns_result_t * results = NULL;
    esp_ip6_addr_t addr6;
    esp_ip4_addr_t addr4;
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());

    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_ptr(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, CONFIG_MDNS_MAX_SERVICES,  &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_srv(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_txt(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, mdns_query_a(MDNS_HOSTNAME, 10, &addr4) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, mdns_query_aaaa(MDNS_HOSTNAME, 10, &addr6) );
    mdns_query_results_free(results);

    mdns_free();
    esp_event_loop_delete_default();
}