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

#ifndef _HCI_AUDIO_H_
#define _HCI_AUDIO_H_
#include <stdint.h>

// Audio state definitions.
typedef enum {
    SCO_STATE_OFF = 0,      // Audio is off.
    SCO_STATE_OFF_TRANSFER, // Closed pending final transfer of audio.
    SCO_STATE_ON,           // Audio is on.
    SCO_STATE_SETUP,        // Open pending completion of audio setup.
} sco_state_t;

// Codec type definitions.
typedef enum {
    SCO_CODEC_NONE = 0x0000,
    SCO_CODEC_CVSD = 0x0001,
    SCO_CODEC_MSBC = 0x0002,
} sco_codec_t;

// Set the audio state on the controller for SCO (PCM, WBS, ...) using the
// vendor library.
void set_audio_state(uint16_t handle, sco_codec_t codec, sco_state_t state);

#endif /* _HCI_AUDIO_H_ */
