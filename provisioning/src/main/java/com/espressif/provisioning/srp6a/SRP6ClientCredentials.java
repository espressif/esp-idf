// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;


/**
 * The SRP-6a client credentials sent to the server at 
 * {@link SRP6ClientSession.State#STEP_2 step two}. These consist of the public 
 * client value 'A' and the client evidence message 'M1'.
 *
 * @author Vladimir Dzhuvinov
 */
public final class SRP6ClientCredentials {


	/**
	 * The public client value 'A'.
	 */
	public final BigInteger A;
	
	
	/**
	 * The client evidence message 'M1'.
	 */
	public final BigInteger M1;
	
	
	/**
	 * Creates a new SRP-6a client credentials.
	 *
	 * @param A  The public client value 'A'. Must not be {@code null}.
	 * @param M1 The client evidence message 'M1'. Must not be {@code null}.
	 */
	public SRP6ClientCredentials(final BigInteger A, final BigInteger M1) {
	
		if (A == null)
			throw new IllegalArgumentException("The public client value 'A' must not be null");
			
		this.A = A;
		
		
		if (M1 == null)
			throw new IllegalArgumentException("The client evidence message 'M1' must not be null");
			
		this.M1 = M1;
	}
}
