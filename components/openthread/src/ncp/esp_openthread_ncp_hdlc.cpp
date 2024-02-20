/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/new.hpp"
#include "ncp_hdlc.hpp"

#if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK

namespace ot {
namespace Ncp {

static OT_DEFINE_ALIGNED_VAR(sNcpRaw, sizeof(NcpHdlc), uint64_t);

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

} // namespace Ncp
} // namespace ot

#endif // #if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
