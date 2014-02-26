package com.java.dataManager;

import com.utils.IOTP;

/**G�n�rateur token
 Attributs: -nom du Token
 -cl� de G�n�ration
 -m�thode de G�n�ration (HOTP/TOTP)
 -longueur du Token
 */

public class Token  {

	/**
	 * nom du Token
	 */
	private String nom = "";

	/**
	 * m�thode de g�n�ration � utiliser (par d�faut HOTP)
	 */
	private int method_Type = OTPMethodType.HOTP;

	
	/**
	 * g�n�rateur otp
	 */
	private IOTP iotp_gen ;

	
	/**
	 * constructeur du token 
	 * @return token
	 */
	public void token(String nom, int method_type, IOTP iotp_gen) {

		this.nom = nom;
		this.method_Type = method_type;
		this.iotp_gen = iotp_gen;
	}

    /**
     * Cette fonction retourne le nom du token
     * @return nom
     */
	public String getNom() {
		return nom;
	}

	
    /**
     * Cette fonction retourne le type de g�r�rateur token
     * utilis� (HOTP ou TOTP)
     * @return O pour HOTP
     * @return 1 pour TOTP
     */
	public int getMethod_Type() {
		return method_Type;
	}


    /**
     * Cette fonction retourne le g�r�rateur token
     * utilis� (HOTP ou TOTP)
     * @return IOTP
     */
	public IOTP getIotp_gen() {
		return iotp_gen;
	}
	
	
	/**
	 * cette fonction toString() retournera cet objet Token 
	 * sous forme de chaine de caract�re
	 * @return String
	 */
	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return super.toString();
	}

}
