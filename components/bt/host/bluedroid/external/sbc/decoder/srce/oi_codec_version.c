/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
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

/**
@file
This file contains a single function, which returns a string indicating the
version number of the eSBC codec

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/
#include "common/bt_target.h"
#include "oi_stddefs.h"
#include "oi_codec_sbc_private.h"

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
/** Version string for the BLUEmagic 3.0 protocol stack and profiles */
PRIVATE OI_CHAR *const codecVersion = "v1.5"
#ifdef OI_SBC_EVAL
                                      " (Evaluation version)"
#endif
                                      ;

/** This function returns the version string for the BLUEmagic 3.0 protocol stack
    and profiles */
OI_CHAR *OI_CODEC_Version(void)
{
    return codecVersion;
}

/**********************************************************************************/

/**
@}
*/

#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
