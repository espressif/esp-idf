// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.espressif.provisioning.srp6a;

import java.math.BigInteger;
import java.security.MessageDigest;

/**
 * Custom routine for computing the client evidence message 'M1'.
 */
public class ClientSRP6Routines implements ClientEvidenceRoutine {

    private MessageDigest digest;

    ClientSRP6Routines(MessageDigest digest)  {
        this.digest = digest;
    }

    @Override
    /**
     * Computes the client evidence message M1 as per RFC 5054.
     *
     * Client evidence message M1 = H[H(N) XOR H(g) | H(I) | s | A | B | KCarol]
     *
     * @param cryptoParams The crypto parameters for the SRP-6a protocol.
     * @param ctx          Snapshot of the SRP-6a client session variables
     *                     which may be used in the computation of the
     *                     client evidence message.
     *
     * @return The resulting client evidence message 'M1'.
     */
    public BigInteger computeClientEvidence(SRP6CryptoParams cryptoParams, SRP6ClientEvidenceContext ctx) {

        byte[] big_N = BigIntegerUtils.bigIntegerToBytes(cryptoParams.N);
        byte[] big_g = BigIntegerUtils.bigIntegerToBytes(cryptoParams.g);

        int padding = big_N.length - big_g.length;

        digest.update(big_N);
        byte[] hn = digest.digest();

        if (big_g.length < big_N.length) {

            byte[] tmp = new byte[big_N.length];
            System.arraycopy(big_g, 0, tmp, padding, big_g.length);
            big_g = tmp;
        }

        digest.update(big_g);
        byte[] hg = digest.digest();

        byte[] c = new byte[hn.length];

        for (int i = 0; i < hn.length; i++) {
            c[i] = (byte) (hn[i] ^ hg[i]);
        }

        digest.update(ctx.userID.getBytes());
        byte[] hi = digest.digest();

        digest.update(c);
        digest.update(hi);
        digest.update(BigIntegerUtils.bigIntegerToBytes(ctx.s));
        digest.update(BigIntegerUtils.bigIntegerToBytes(ctx.A));
        digest.update(BigIntegerUtils.bigIntegerToBytes(ctx.B));
        digest.update(BigIntegerUtils.bigIntegerToBytes(ctx.K));

        return BigIntegerUtils.bigIntegerFromBytes(digest.digest());
    }
}
