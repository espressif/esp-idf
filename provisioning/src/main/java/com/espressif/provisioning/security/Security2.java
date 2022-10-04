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

package com.espressif.provisioning.security;

import android.util.Log;

import com.espressif.provisioning.srp6a.BigIntegerUtils;
import com.espressif.provisioning.srp6a.SRP6ClientCredentials;
import com.espressif.provisioning.srp6a.SRP6ClientSession;
import com.espressif.provisioning.srp6a.SRP6CryptoParams;
import com.espressif.provisioning.srp6a.SRP6Exception;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;

import java.math.BigInteger;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import espressif.Sec2;
import espressif.Session;

/**
 * Security 1 implementation of the handshake and encryption
 * protocols.
 * Security 1 is based on AES CTR mode with NoPadding
 */
public class Security2 implements Security {
    private static final String TAG = "Espressif::" + Security2.class.getSimpleName();

    private static final int SESSION_STATE_REQUEST1 = 0;
    private static final int SESSION_STATE_RESPONSE1_REQUEST2 = 1;
    private static final int SESSION_STATE_RESPONSE2 = 2;
    private static final int SESSION_STATE_FINISHED = 3;

    private int sessionState = SESSION_STATE_REQUEST1;

    private String userName;

    private Cipher cipher;
    private SRP6ClientSession client;

    private BigInteger clientPublicKey = null;
    private BigInteger devicePublicKey = null;
    private byte[] deviceNonce;
    private byte[] clientProof;
    private byte[] sharedKey;
    private byte[] key;

    /***
     * Create Security 1 implementation
     */
    public Security2(String username, String password) {

        userName = username;
        Log.d(TAG, "User name : " + username + " password : " + password);

        try {
            this.cipher = Cipher.getInstance("AES/GCM/NoPadding");
        } catch (NoSuchAlgorithmException | NoSuchPaddingException e) {
            e.printStackTrace();
        }
        client = new SRP6ClientSession();
        client.step1(username, password);
    }

    @Override
    public byte[] getNextRequestInSession(byte[] hexData) {
        byte[] request = null;
        switch (this.sessionState) {
            case SESSION_STATE_REQUEST1:
                this.sessionState = SESSION_STATE_RESPONSE1_REQUEST2;
                request = this.getStep0Request();
                break;
            case SESSION_STATE_RESPONSE1_REQUEST2:
                this.sessionState = SESSION_STATE_RESPONSE2;
                this.processStep0Response(hexData);
                request = this.getStep1Request();
                break;
            case SESSION_STATE_RESPONSE2:
                this.sessionState = SESSION_STATE_FINISHED;
                this.processStep1Response(hexData);
                break;
            default:
                break;
        }
        return request;
    }

    private byte[] getStep0Request() {
        try {
            try {
                clientPublicKey = client.getClientPublicKey(SRP6CryptoParams.getInstance(3072, "SHA-512"));
            } catch (SRP6Exception e) {
                e.printStackTrace();
            }
            byte[] publicKeyA = BigIntegerUtils.bigIntegerToBytes(clientPublicKey);

            Sec2.S2SessionCmd0 sessionCmd0 = Sec2.S2SessionCmd0
                    .newBuilder()
                    .setClientUsername(ByteString.copyFrom(userName.getBytes()))
                    .setClientPubkey(ByteString.copyFrom(publicKeyA))
                    .build();
            Sec2.Sec2Payload sec2Payload = Sec2.Sec2Payload
                    .newBuilder()
                    .setSc0(sessionCmd0)
                    .build();
            Session.SessionData sessionData = Session.SessionData
                    .newBuilder()
                    .setSecVer(Session.SecSchemeVersion.SecScheme2)
                    .setSec2(sec2Payload)
                    .build();
            return sessionData.toByteArray();
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
            e.printStackTrace();
        }
        return null;
    }

    private byte[] getStep1Request() {

        Sec2.S2SessionCmd1 sessionCmd1 = Sec2.S2SessionCmd1
                .newBuilder()
                .setClientProof(ByteString.copyFrom(clientProof))
                .build();
        Sec2.Sec2Payload sec1Payload = Sec2.Sec2Payload
                .newBuilder()
                .setSc1(sessionCmd1)
                .setMsg(Sec2.Sec2MsgType.S2Session_Command1)
                .build();
        Session.SessionData sessionData = Session.SessionData
                .newBuilder()
                .setSecVer(Session.SecSchemeVersion.SecScheme2)
                .setSec2(sec1Payload)
                .build();
        return sessionData.toByteArray();
    }

    private void processStep0Response(byte[] hexData) throws RuntimeException {

        try {
            if (hexData == null) {
                throw new RuntimeException("No response from device");
            }
            Session.SessionData responseData = Session.SessionData.parseFrom(hexData);
            if (responseData.getSecVer() != Session.SecSchemeVersion.SecScheme2) {
                throw new RuntimeException("Security version mismatch");
            }

            byte[] deviceSalt = responseData.getSec2().getSr0().getDeviceSalt().toByteArray();
            byte[] devicePubKey = responseData.getSec2().getSr0().getDevicePubkey().toByteArray();

            BigInteger salt = BigIntegerUtils.bigIntegerFromBytes(deviceSalt);
            devicePublicKey = BigIntegerUtils.bigIntegerFromBytes(devicePubKey);

            try {
                SRP6CryptoParams params = SRP6CryptoParams.getInstance(3072, "SHA-512");
                SRP6ClientCredentials clientCredentials = client.step2_for_client_evidence(params, salt, devicePublicKey, deviceSalt);
                BigInteger M1 = clientCredentials.M1;
                clientProof = BigIntegerUtils.bigIntegerToBytes(M1);
            } catch (SRP6Exception e) {
                e.printStackTrace();
            }
        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void processStep1Response(byte[] hexData) throws RuntimeException {
        try {
            if (hexData == null) {
                throw new RuntimeException("No response from device");
            }

            Session.SessionData responseData = Session.SessionData.parseFrom(hexData);
            if (responseData.getSecVer() != Session.SecSchemeVersion.SecScheme2) {
                throw new RuntimeException("Security version mismatch");
            }

            byte[] deviceProof = responseData.getSec2().getSr1().getDeviceProof().toByteArray();
            deviceNonce = responseData.getSec2().getSr1().getDeviceNonce().toByteArray();

            try {
                client.step3(BigIntegerUtils.bigIntegerFromBytes(deviceProof));
            } catch (SRP6Exception e) {
                e.printStackTrace();
            }

            sharedKey = BigIntegerUtils.bigIntegerToBytes(client.K);
            key = Arrays.copyOfRange(sharedKey, 0, 32);

        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public byte[] encrypt(byte[] data) {

        // Device nonce = IV
        SecretKeySpec secretKeySpec = new SecretKeySpec(key, "AES");
        IvParameterSpec parameterSpec = new IvParameterSpec(deviceNonce);
        try {
            this.cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, parameterSpec);
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        }

        try {
            return this.cipher.doFinal(data);
        } catch (BadPaddingException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        }
        return null;
    }

    public byte[] decrypt(byte[] data) {

        SecretKeySpec secretKeySpec = new SecretKeySpec(key, "AES");
        IvParameterSpec parameterSpec = new IvParameterSpec(deviceNonce);
        try {
            this.cipher.init(Cipher.DECRYPT_MODE, secretKeySpec, parameterSpec);
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        }

        try {
            return this.cipher.doFinal(data);
        } catch (BadPaddingException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        }
        return null;
    }
}
