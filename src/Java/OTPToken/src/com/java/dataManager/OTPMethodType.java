package com.java.dataManager;

/**
 * Interface cat�gorisant les 2 types de g�n�ration d'OTP utilis�e
 * @author ADEGOLOYE Yves
 */
public interface OTPMethodType {
	
	/**
	 * OTP Method Type HOTP
	 */
	int HOTP =0;
	
	
	
	/**
	 * OTP Method Type HOTP
	 */
	int TOTP=1;
}
