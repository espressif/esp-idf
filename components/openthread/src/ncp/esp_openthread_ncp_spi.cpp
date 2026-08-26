/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/new.hpp"
#include "ncp_spi.hpp"

namespace ot {
namespace Ncp {

static OT_DEFINE_ALIGNED_VAR(sNcpRaw, sizeof(NcpSpi), uint64_t);

extern "C" void otNcpSpiInit(otInstance *aInstance)
{
    NcpSpi   *ncpSpi   = nullptr;
    Instance *instance = static_cast<Instance *>(aInstance);

    ncpSpi = new (&sNcpRaw) NcpSpi(instance);

    if (ncpSpi == nullptr || ncpSpi != NcpBase::GetNcpInstance())
    {
        OT_ASSERT(false);
    }
}

} // namespace Ncp
} // namespace ot
