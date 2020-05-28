// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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


package com.espressif.provisioning.security;

import android.util.Log;

import com.google.protobuf.InvalidProtocolBufferException;

import espressif.Sec0;
import espressif.Session;

/**
 * Security 0 implementation of the handshake and encryption
 * protocol.
 * Security 0 specifies no encryption and only a single step handshake.
 */
public class Security0 implements Security {
    private static final String TAG = "Espressif::" + Security1.class.getSimpleName();

    private static final int SESSION_STATE_0 = 0;
    private static final int SESSION_STATE_1 = 1;

    private int sessionState = SESSION_STATE_0;

    @Override
    public byte[] getNextRequestInSession(byte[] hexData) {
        byte[] response = null;
        switch (sessionState) {
            case SESSION_STATE_0:
                this.sessionState = SESSION_STATE_1;
                response = this.getStep0Request();
                break;
            case SESSION_STATE_1:
                this.processStep0Response(hexData);
                break;
        }

        return response;
    }

    @Override
    public byte[] encrypt(byte[] data) {
        return data;
    }

    @Override
    public byte[] decrypt(byte[] data) {
        return data;
    }

    private byte[] getStep0Request() {
        Sec0.S0SessionCmd s0SessionCmd = Sec0.S0SessionCmd
                .newBuilder()
                .build();
        Sec0.Sec0Payload sec0Payload = Sec0.Sec0Payload
                .newBuilder()
                .setSc(s0SessionCmd)
                .build();
        Session.SessionData newSessionData = Session.SessionData
                .newBuilder()
                .setSecVerValue(0)
                .setSec0(sec0Payload)
                .build();
        return newSessionData.toByteArray();
    }

    private void processStep0Response(byte[] hexData) throws RuntimeException {
        try {
            if (hexData == null) {
                throw new RuntimeException("No response from device");
            }

            Session.SessionData responseData = Session.SessionData.parseFrom(hexData);
            if (responseData.getSecVer() != Session.SecSchemeVersion.SecScheme0) {
                throw new RuntimeException("Security version mismatch");
            }
        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, e.getMessage());
        }
    }
}
