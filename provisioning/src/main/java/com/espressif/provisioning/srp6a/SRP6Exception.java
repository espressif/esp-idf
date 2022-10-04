// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


/**
 * Secure Remote Password (SRP-6a) exception.
 *
 * @author Vladimir Dzhuvinov
 */
public class SRP6Exception extends Exception {


	private static final long serialVersionUID = 4640494990301260666L;


	/**
	 * SRP-6a exception causes.
	 */
	public static enum CauseType {
	
	
		/**
		 * Invalid public client or server value ('A' or 'B').
		 */
		BAD_PUBLIC_VALUE,
		
		
		/**
		 * Invalid credentials (password).
		 */
		BAD_CREDENTIALS,
		
		
		/**
		 * SRP-6a authentication session timeout.
		 */
		TIMEOUT
	}
	
	
	/**
	 * The cause type.
	 */
	private CauseType cause;


	/**
	 * Creates a new Secure Remote Password (SRP-6a) exception with the 
	 * specified message.
	 *
	 * @param message The exception message.
	 * @param cause   The exception cause type. Must not be {@code null}.
	 */
	public SRP6Exception(final String message, final CauseType cause) {
	
		super(message);
		
		if (cause == null)
			throw new IllegalArgumentException("The cause type must not be null");
		
		this.cause = cause;
	}
	
	
	/**
	 * Gets the cause type for this exception.
	 *
	 * @return The exception cause type.
	 */
	public CauseType getCauseType() {
	
		return cause;
	}
}
