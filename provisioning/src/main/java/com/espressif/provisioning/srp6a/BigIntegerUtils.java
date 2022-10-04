// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;
import java.util.Arrays;


/**
 * Hexadecimal encoding and decoding utility.
 *
 * <p>Obtained from Apache Xerces and Aduna Software code on java2s.com.
 *
 * @author Vladimir Dzhuvinov
 * @author John Kim
 * @author others
 */
public class BigIntegerUtils {

	/**
	 * Encodes the specified big integer into a hex string.
	 *
	 * @param bigint the value to convert to a hexidecimal string
	 *
	 * @return The resulting hex encoded string or {@code null} if the
	 *         input is undefined.
	 */
	public static String toHex(final BigInteger bigint) {

		if (bigint == null)
			return null;

		return bigint.toString(16);
	}

	/**
	 * Decodes the specified hex string into a big integer.
	 *
	 * @param hex The hex encoded string to decode.
	 *
	 * @return The resulting big integer or {@code null} if decoding
	 *         failed.
	 */
	public static BigInteger fromHex(final String hex) {

		if (hex == null)
			return null;

		try {
			return new BigInteger(hex, 16);

		} catch (NumberFormatException e) {

			return null;
		}
	}

	/**
	 * Converts a byte array to a positive BigInteger
	 *
	 * @param bytes byte array in big endian unsigned RFC2945 format
	 *
	 * @return positive BigInteger containing the data of the supplied byte array
	 */
	public static BigInteger bigIntegerFromBytes(final byte[] bytes) {
		return new BigInteger(1, bytes);
	}

	/**
	 * Converts a BigInteger into a byte array ignoring the sign of the BigInteger, according to RFC2945 specification
	 *
	 * @param bigInteger BigInteger, must not be null, should not be negative
	 *
	 * @return byte array (leading byte is always != 0), empty array if BigInteger is zero.
	 */
	public static byte[] bigIntegerToBytes(final BigInteger bigInteger) {
		assert(bigInteger.signum() != -1); // NOSONAR
		byte[] bytes = bigInteger.toByteArray();
		if (bytes[0] == 0) {
			return Arrays.copyOfRange(bytes, 1, bytes.length);
		}
		return bytes;
	}

	/**
	 * Prevents instantiation.
	 */
	private BigIntegerUtils() {

		// do nothing
	}
}
