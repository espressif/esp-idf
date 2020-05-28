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


package com.espressif.provisioning.utils;

/**
 * Util class for manipulating byte Arrays and
 * their Hex string representations
 */

public class HexEncoder {

    public static String byteArrayToHexString(byte[] input) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < input.length; i++) {
            sb.append(Character.forDigit((input[i] >> 4) & 0xF, 16));
            sb.append(Character.forDigit((input[i] & 0xF), 16));
        }
        return sb.toString();
    }

    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    public static byte[] xor(byte[] first, byte[] second) {
        int maxLen = Math.max(first.length, second.length);
        byte[] output = new byte[maxLen];

        for (int i = 0; i < maxLen; i++) {
            int ordA = (int) first[(i % first.length)];
            int ordB = (int) second[(i % second.length)];
            output[i] = (byte) (ordA ^ ordB);
        }

        return output;
    }
}
