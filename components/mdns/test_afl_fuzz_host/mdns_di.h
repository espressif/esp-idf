/*
 * MDNS Dependecy injection -- preincluded to inject interface test functions into static variables
 * 
 */

#include "mdns.h"
#include "mdns_private.h"

void              (*mdns_test_static_execute_action)(mdns_action_t *) = NULL;
mdns_srv_item_t * (*mdns_test_static_mdns_get_service_item)(const char * service, const char * proto) = NULL;
mdns_search_once_t * (*mdns_test_static_search_init)(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, uint8_t max_results) = NULL;
esp_err_t         (*mdns_test_static_send_search_action)(mdns_action_type_t type, mdns_search_once_t * search) = NULL;
void              (*mdns_test_static_search_free)(mdns_search_once_t * search) = NULL;

static void _mdns_execute_action(mdns_action_t * action);
static mdns_srv_item_t * _mdns_get_service_item(const char * service, const char * proto);
static mdns_search_once_t * _mdns_search_init(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, uint8_t max_results);
static esp_err_t _mdns_send_search_action(mdns_action_type_t type, mdns_search_once_t * search);
static void _mdns_search_free(mdns_search_once_t * search);

void mdns_test_init_di()
{
    mdns_test_static_execute_action = _mdns_execute_action;
    mdns_test_static_mdns_get_service_item = _mdns_get_service_item;
    mdns_test_static_search_init = _mdns_search_init;
    mdns_test_static_send_search_action = _mdns_send_search_action;
    mdns_test_static_search_free = _mdns_search_free;
}

void mdns_test_execute_action(void * action)
{
    mdns_test_static_execute_action((mdns_action_t *)action);
}

void mdns_test_search_free(mdns_search_once_t * search)
{
    return mdns_test_static_search_free(search);
}

esp_err_t mdns_test_send_search_action(mdns_action_type_t type, mdns_search_once_t * search)
{
    return mdns_test_static_send_search_action(type, search);
}

mdns_search_once_t * mdns_test_search_init(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, uint8_t max_results)
{
    return mdns_test_static_search_init(name, service, proto, type, timeout, max_results);
}

mdns_srv_item_t * mdns_test_mdns_get_service_item(const char * service, const char * proto)
{
    return mdns_test_static_mdns_get_service_item(service, proto);
}