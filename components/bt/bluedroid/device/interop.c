/******************************************************************************
 *
 *  Copyright (C) 2015 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/*
#define LOG_TAG "bt_device_interop"
*/
#include <string.h> // For memcmp
#include "bt_trace.h"
#include "bdaddr.h"
#include "interop.h"
#include "interop_database.h"

#define CASE_RETURN_STR(const) case const: return #const;

#if (SMP_INCLUDED == TRUE)
static const char *interop_feature_string(const interop_feature_t feature)
{
    switch (feature) {
        CASE_RETURN_STR(INTEROP_DISABLE_LE_SECURE_CONNECTIONS)
        CASE_RETURN_STR(INTEROP_AUTO_RETRY_PAIRING)
    }

    return "UNKNOWN";
}

// Interface functions
bool interop_match(const interop_feature_t feature, const bt_bdaddr_t *addr)
{
    assert(addr);

    const size_t db_size = sizeof(interop_database) / sizeof(interop_entry_t);

    for (size_t i = 0; i != db_size; ++i) {
        if (feature == interop_database[i].feature &&
                memcmp(addr, &interop_database[i].addr, interop_database[i].len) == 0) {
            char bdstr[20] = {0};
            LOG_WARN("%s() Device %s is a match for interop workaround %s", __func__,
                     bdaddr_to_string(addr, bdstr, sizeof(bdstr)), interop_feature_string(feature));
            return true;
        }
    }

    return false;
}
#endif  ///SMP_INCLUDED == TRUE
