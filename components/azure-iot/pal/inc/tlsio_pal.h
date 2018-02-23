// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TLSIO_PAL_H
#define TLSIO_PAL_H

#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, tlsio_pal_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_PAL_H */
