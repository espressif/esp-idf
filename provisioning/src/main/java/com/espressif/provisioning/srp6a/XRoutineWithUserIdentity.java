// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;
import java.security.MessageDigest;


/**
 * Alternative routine for computing a password key x = H(s | H(I | ":" | P))
 * 
 * <p>Specification: RFC 5054.
 *
 * <p>This routine can be passed to the {@link SRP6VerifierGenerator} and
 * {@link SRP6ClientSession} to replace the
 * {@link SRP6Routines#computeX default routine} x = H(s | H(P)).
 *
 * @author Vladimir Dzhuvinov
 */
public class XRoutineWithUserIdentity implements XRoutine {


	/**
	 * Computes x = H(s | H(I | ":" | P))
	 *
	 * <p>This method complies with the RFC 5054 recommendation, save for 
	 * the hash algorithm which can be any (RFC 5054 recommends SHA-1).
	 *
	 * @param digest   The hash function 'H'. Must not be {@code null}.
	 * @param salt     The salt 's'. Must not be {@code null}.
	 * @param username The user identity 'I'. Must not be {@code null}.
	 * @param password The user password 'P'. Must not be {@code null}.
	 *
	 * @return The resulting 'x' value.
	 */
	public BigInteger computeX(final MessageDigest digest,
				   final byte[] salt,
				   final byte[] username,
				   final byte[] password) {
	
		digest.update(username);
		digest.update((byte)':');
		digest.update(password);
		
		byte[] output = digest.digest();
		
		digest.update(salt);
		output = digest.digest(output);

		return BigIntegerUtils.bigIntegerFromBytes(output);
	}
	
	
	/**
	 * Returns a string representation of this routine algorithm.
	 * 
	 * @return "H(s | H(I | ":" | P))"
	 */
	public String toString() {
	
		return "H(s | H(I | \":\" | P))";
	}
}
