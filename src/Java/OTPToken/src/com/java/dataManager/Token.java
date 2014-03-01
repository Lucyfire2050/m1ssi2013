package com.java.dataManager;

import org.simpleframework.xml.Element;
import org.simpleframework.xml.Root;
import com.java.utils.HOTP;
import com.java.utils.IOTP;
import com.java.utils.OTPGenerator;
import com.java.utils.Secret;
import com.java.utils.TOTP;



/**G�n�rateur token
 Attributs: -nom du Token
 -cl� de G�n�ration
 -m�thode de G�n�ration (HOTP/TOTP)
 -longueur du Token
 */

@Root
public class Token  {

	/**
	 * nom du Token
	 */
	@Element
	private String nom = "";

	/**
	 * type de g�n�ration utilis� (par d�faut HOTP)
	 */
	@Element
	private int method_Type = OTPMethodType.HOTP;
	
	
	/**
	 * taille de l'OTP
	 */
	@Element
	private int tailleOTP=0; 
	
	
	/**
	 * compteur actuel du token si m�thod_type=HOTP, 0 sinon
	 */
	@Element
	private long count=0;
	
	
	/**
	 * temps de validit� du token si m�thod_type=TOTP, 0 sinon
	 */
	@Element
	private int quantum=0;
	
	
	
	/**
	 * cl� de hashage utilis�e
	 */
	@Element
	private String skey_hex = "";
	

	
	/**
	 * g�n�rateur otp
	 */
	private IOTP iotp_gen ;

	
	/**
	 * constructeur du token 
	 * @return token
	 */
	public Token(String nom, int method_type, IOTP iotp_gen) {

		this.nom = nom;
		this.method_Type = method_type;
		this.iotp_gen = iotp_gen;
		this.tailleOTP= ((OTPGenerator)iotp_gen).getDigits();
		this.skey_hex= ((OTPGenerator)iotp_gen).getKey().getHexRepresentation();
	}
	
	
	
	/**
	 * constructeur par d�faut (utille pour la d�s�rialisation)
	 */
	public Token(){	
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
	 * c'est une fonction propre au token qui appelle implicitement son g�n�rateur IOTP
	 * @return
	 */
	public int generate(){
		if(method_Type==OTPMethodType.HOTP){
			if(iotp_gen ==null){
				Secret secret = new Secret();
				secret.setSecret(skey_hex);
				iotp_gen = new HOTP(count, secret, tailleOTP);
			}
		}else{
			if(iotp_gen ==null){
				Secret secret = new Secret();
				secret.setSecret(skey_hex);
				iotp_gen = new TOTP(secret, tailleOTP,quantum);
			}
		}

		int resultat = iotp_gen.generer();
		
		if(method_Type==OTPMethodType.HOTP){
        count =((OTPGenerator)iotp_gen).getCount();
		}	
		return resultat;
	}

}
