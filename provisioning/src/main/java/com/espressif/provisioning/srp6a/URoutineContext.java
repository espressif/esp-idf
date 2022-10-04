// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;


/**
 * Immutable snapshot of the SRP-6a client session variables to be used in a
 * {@link URoutine}.
 *
 * @author Simon Massey
 */
public class URoutineContext {


	/**
	 * The public client value 'A'.
	 */
	public final BigInteger A;


	/**
	 * The public server value 'B'.
	 */
	public final BigInteger B;


	/**
	 * Creates a new immutable snapshot of SRP-6a client session variables
	 * to be used in a {@link URoutine}.
	 *
	 * @param A The public client value 'A'.
	 * @param B The public server value 'B'.
	 */
	public URoutineContext(final BigInteger A, final BigInteger B) {

		this.A = A;
		this.B = B;
	}
}
