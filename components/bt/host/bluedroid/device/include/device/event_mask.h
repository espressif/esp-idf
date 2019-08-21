/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
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
#ifndef _EVENT_MASK_H_
#define _EVENT_MASK_H_

#include <stdint.h>

// Represents a mask which can be used to tell the controller which
// HCI events the stack wishes to be informed about. See the bluetooth
// spec for more information on what each bit means.
typedef struct {
    uint8_t as_array[8];
} bt_event_mask_t;

#endif /*_EVENT_MASK_H_*/
