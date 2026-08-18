/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "common/new.hpp"
#include "ncp_hdlc.hpp"

namespace ot {
namespace Ncp {

static OT_DEFINE_ALIGNED_VAR(sNcpRaw, sizeof(NcpHdlc), uint64_t);

#if CONFIG_OPENTHREAD_MULTIPAN_RCP_ENABLE
extern "C" void otNcpHdlcInitMulti(otInstance **aInstances, uint8_t aCount, otNcpHdlcSendCallback aSendCallback)
{
    NcpHdlc      *ncpHdlc = nullptr;
    ot::Instance *instances[SPINEL_HEADER_IID_MAX];

    OT_ASSERT(aCount < SPINEL_HEADER_IID_MAX + 1);
    OT_ASSERT(aCount > 0);
    OT_ASSERT(aInstances[0] != nullptr);

    for (int i = 0; i < aCount; i++)
    {
        OT_ASSERT(aInstances[i] != nullptr);
        instances[i] = static_cast<ot::Instance *>(aInstances[i]);
    }

    ncpHdlc = new (&sNcpRaw) NcpHdlc(instances, aCount, aSendCallback);

    if (ncpHdlc == nullptr || ncpHdlc != NcpBase::GetNcpInstance())
    {
        OT_ASSERT(false);
    }
}
#else
extern "C" void otNcpHdlcInit(otInstance *aInstance, otNcpHdlcSendCallback aSendCallback)
{
    NcpHdlc  *ncpHdlc  = nullptr;
    Instance *instance = static_cast<Instance *>(aInstance);

    ncpHdlc = new (&sNcpRaw) NcpHdlc(instance, aSendCallback);

    if (ncpHdlc == nullptr || ncpHdlc != NcpBase::GetNcpInstance())
    {
        OT_ASSERT(false);
    }
}
#endif

} // namespace Ncp
} // namespace ot
