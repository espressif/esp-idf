/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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

#ifndef _SBC_IF_H
#define _SBC_IF_H

#define PCM_BUFFER_SIZE 512

/*
 SBC_Init - called once for each track played

 pcm_sample_freq - 4000 to 48000
 channels - 1 mono 2 stereo
 bits_per_sample - 8 or 16
 return - 0 sucess
*/

int SBC_init(int pcm_sample_freq, int channels, int bits_per_sample);

/*
 SBC_write - called repeatedly with pcm_in pointer
    increasing by length until track is finished.

 pcm_in - pointer to PCM buffer
 length - any
 sbc_out - pointer to SBC output buffer
 return - number of bytes written to sbc_out
*/

int SBC_write(unsigned char *pcm_in, int length, unsigned char *sbc_out);

#endif
