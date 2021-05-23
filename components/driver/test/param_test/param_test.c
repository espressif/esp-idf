/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include "param_test.h"
#include "esp_log.h"

void test_serializer(const param_group_t *param_group, const ptest_func_t* test_func)
{
    ESP_LOGI("test", "run test: %s", param_group->name);
    //in this test case, we want to make two devices as similar as possible, so use the same context
    void *context = NULL;
    test_func->pre_test(&context);

    void *pset = param_group->param_group;
    for (int i = param_group->pset_num; i >0; i--) {
        if (test_func->def_param) test_func->def_param(pset);
        test_func->loop(pset, context);
        pset+=param_group->pset_size;
    }

    test_func->post_test(context);
    free(context);
    context = NULL;
}
