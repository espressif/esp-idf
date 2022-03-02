/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes compile-time configuration constants for OpenThread.
 */

#ifndef OPENTHREAD_SPINEL_CONFIG_H_
#define OPENTHREAD_SPINEL_CONFIG_H_

/**
 * @def OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
 *
 * Define 1 to enable feeding an OpenThread message to encoder/decoder.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
#define OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE 0
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
 *
 * Defines the max count of RCP failures allowed to be recovered.
 * 0 means to disable RCP failure recovering.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 0
#endif

/**
 * @def OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION
 *
 * Define 1 to call the custom RCP failure handler on RCP failure.
 *
 */
#ifndef OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION
#define OPENTHREAD_SPINEL_CONFIG_RCP_CUSTOM_RESTORATION 0
#endif

#endif // OPENTHREAD_SPINEL_CONFIG_H_
