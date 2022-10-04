// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;
import java.security.MessageDigest;


/**
 * Custom routine interface for computing the password key 'x'. Calculation of 
 * this value is required by the verifier 'v' generator as well as by the 
 * authenticating {@link SRP6ClientSession client session}.
 *
 * <p>If you don't want to employ the {@link SRP6Routines#computeX default routine} 
 * for computing 'x' you can use this interface to define your own. Remember to 
 * make sure that exactly the same routine is used to generate the verifier 'v' 
 * and by client authentication sessions later, else authentication will fail.
 *
 * <p>For a sample implementation see {@link XRoutineWithUserIdentity} which
 * computes x = H(s | H(I | ":" | P))
 *
 * @author Vladimir Dzhuvinov
 */
public interface XRoutine {


	/**
	 * Computes the password key 'x'.
	 *
	 * <p>Tip: To convert a string to a byte array you can use 
	 * {@code String.getBytes()} or 
	 * {@code String.getBytes(java.nio.charset.Charset)}. To convert a big
	 * integer to a byte array you can use {@code BigInteger.toByteArray()}.
	 *
	 * @param digest   The hash function 'H'. To enforce a particular hash
	 *                 algorithm, e.g. "SHA-1", you may perform a check 
	 *                 that throws an {@code IllegalArgumentException} or
	 *                 ignore this argument altogether.
	 * @param salt     The salt 's'. This is considered a mandatory
	 *                 argument in computation of 'x'.
	 * @param username The user identity 'I'. It may be ignored if the 
	 *                 username is allowed to change or if a user may 
	 *                 authenticate with multiple alternate identities,
	 *                 such as name and email address.
	 * @param password The user password 'P'. This is considered a
	 *                 mandatory argument in the computation of 'x'.
	 *
	 * @return The resulting 'x' value.
	 */
	public BigInteger computeX(final MessageDigest digest, 
	                           final byte[] salt, 
				   final byte[] username, 
				   final byte[] password);
}
