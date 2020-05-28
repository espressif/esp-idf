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

import com.espressif.provisioning.utils.HexEncoder;
import com.google.crypto.tink.subtle.X25519;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;

import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import javax.crypto.Cipher;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import espressif.Sec1;
import espressif.Session;

/**
 * Security 1 implementation of the handshake and encryption
 * protocols.
 * Security 1 is based on AES CTR mode with NoPadding
 *
 */
public class Security1 implements Security {
    private static final String TAG = "Espressif::" + Security1.class.getSimpleName();

    private static final int SESSION_STATE_REQUEST1 = 0;
    private static final int SESSION_STATE_RESPONSE1_REQUEST2 = 1;
    private static final int SESSION_STATE_RESPONSE2 = 2;
    private static final int SESSION_STATE_FINISHED = 3;

    private int sessionState = SESSION_STATE_REQUEST1;
    private byte[] privateKey = null;
    private byte[] publicKey = null;
    private byte[] proofOfPossession = null;
    private byte[] clientVerify = null;


    private Cipher cipher;

    /***
     * Create Security 1 implementation
     * @param proofOfPossession proof of possession identifying the physical device
     */
    public Security1(String proofOfPossession) {
        if(proofOfPossession != null) {
            this.proofOfPossession = proofOfPossession.getBytes();
        }
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
            this.generateKeyPair();
            Sec1.SessionCmd0 sessionCmd0 = Sec1.SessionCmd0
                    .newBuilder()
                    .setClientPubkey(ByteString.copyFrom(this.publicKey))
                    .build();
            Sec1.Sec1Payload sec1Payload = Sec1.Sec1Payload
                    .newBuilder()
                    .setSc0(sessionCmd0)
                    .build();
            Session.SessionData sessionData = Session.SessionData
                    .newBuilder()
                    .setSecVer(Session.SecSchemeVersion.SecScheme1)
                    .setSec1(sec1Payload)
                    .build();
            return sessionData.toByteArray();
        } catch (InvalidKeyException e) {
            Log.e(TAG, e.getMessage());
        }
        return null;
    }

    private byte[] getStep1Request() {
        Sec1.SessionCmd1 sessionCmd1 = Sec1.SessionCmd1
                .newBuilder()
                .setClientVerifyData(ByteString.copyFrom(this.clientVerify))
                .build();
        Sec1.Sec1Payload sec1Payload = Sec1.Sec1Payload
                .newBuilder()
                .setSc1(sessionCmd1)
                .setMsg(Sec1.Sec1MsgType.Session_Command1)
                .build();
        Session.SessionData sessionData = Session.SessionData
                .newBuilder()
                .setSecVer(Session.SecSchemeVersion.SecScheme1)
                .setSec1(sec1Payload)
                .build();
        return sessionData.toByteArray();
    }

    private void processStep0Response(byte[] hexData) throws RuntimeException {
        try {
            if (hexData == null) {
                throw new RuntimeException("No response from device");
            }
            Session.SessionData responseData = Session.SessionData.parseFrom(hexData);
            if (responseData.getSecVer() != Session.SecSchemeVersion.SecScheme1) {
                throw new RuntimeException("Security version mismatch");
            }

            byte[] devicePublicKey = responseData.getSec1().getSr0().getDevicePubkey().toByteArray();
            byte[] deviceRandom = responseData.getSec1().getSr0().getDeviceRandom().toByteArray();
            byte[] sharedKey = X25519.computeSharedSecret(this.privateKey, devicePublicKey);

            if (this.proofOfPossession.length > 0) {
                MessageDigest md = MessageDigest.getInstance("SHA256");
                md.update(this.proofOfPossession);
                byte[] digest = md.digest();
                sharedKey = HexEncoder.xor(sharedKey, digest);
            }

            IvParameterSpec ivParameterSpec = new IvParameterSpec(deviceRandom);
            SecretKeySpec secretKeySpec = new SecretKeySpec(sharedKey, 0, sharedKey.length, "AES");

            this.cipher = Cipher.getInstance("AES/CTR/NoPadding");
            this.cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, ivParameterSpec);

            this.clientVerify = this.encrypt(devicePublicKey);
        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, e.getMessage());
        } catch (InvalidKeyException e) {
            Log.e(TAG, e.getMessage());
        } catch (NoSuchAlgorithmException e) {
            Log.e(TAG, e.getMessage());
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        }
    }

    private void processStep1Response(byte[] hexData) throws RuntimeException {
        try {
            if (hexData == null) {
                throw new RuntimeException("No response from device");
            }

            Session.SessionData responseData = Session.SessionData.parseFrom(hexData);
            if (responseData.getSecVer() != Session.SecSchemeVersion.SecScheme1) {
                throw new RuntimeException("Security version mismatch");
            }
            byte[] deviceVerifyData = responseData.getSec1().getSr1().getDeviceVerifyData().toByteArray();
            byte[] decryptedDeviceVerifyData = this.decrypt(deviceVerifyData);
            if (!Arrays.equals(this.publicKey, decryptedDeviceVerifyData)) {
                throw new RuntimeException("Session establishment failed !");
            }
        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void generateKeyPair() throws InvalidKeyException {
        this.privateKey = X25519.generatePrivateKey();
        this.publicKey = X25519.publicFromPrivate(this.privateKey);
    }

    public byte[] encrypt(byte[] data) {
        return this.cipher.update(data);
    }

    public byte[] decrypt(byte[] data) {
        return this.cipher.update(data);
    }
}
