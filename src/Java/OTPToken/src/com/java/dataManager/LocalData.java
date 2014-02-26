package com.java.dataManager;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OptionalDataException;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * Cette classe permet de charger et enregistrer les donn�es Utilisateur
 */

public class LocalData implements Serializable {

	
	
	
	/**
	 * serial Version UId g�n�r� par d�faut
	 */
	private static final long serialVersionUID = 1L;

	
	
	
	/**
	 * PIN de l'utisateur
	 */
	private String PIN = "";

	
	
	
	/**
	 * bool�en pr�cisant si c'est la premi�re utilisation de l'application
	 */
	public boolean firstTime = false;

	
	
	/**
	 * liste de token cr�� par l'utilisateur
	 */
	private ArrayList<Token> listeToken = new ArrayList<Token>();

	
	
	
	/**
	 * instance de la classe LocalData
	 */
	private LocalData instance = null;

	
	
	
	/**
	 * constructeur de la classe LocalData
	 */
	private LocalData() {
	}

	
	
	
	/**
	 * Cette fonction permet d'avoir une seule instance de la classe LocalData
	 * cette seule instance pourra �tre utilis�e partout dans l'application
	 * (design patern singleton)
	 */
	public LocalData getInstance() {
		if (instance == null) {
			return new LocalData();
		}
		return instance;
	}

	
	
	
	/**
	 * fonction qui permet de loader les donn�es chiff�es, de d�chiffrer les
	 * donn�es utilisateur, et d'en cr�er une instance de la classe LocalData
	 */
	public void load() {
	}

	
	
	
	/**
	 * fonction qui permet de chiffrer et d'enregistrer les donn�es utilisateur
	 */
	public void commit() {
	}

	
	
	
	/**
	 * Cette fonction retourne la liste des Tokens de l'utilisateur
	 * 
	 * @return listeToken
	 */
	public ArrayList<Token> getListeToken() {
		return listeToken;
	}

	
	
	
	/**
	 * Cette fonction retourne un token en prenant comme param�tre son nom
	 * 
	 * @param nom
	 * @return token
	 */
	public Token getToken(String nom) {
		return null;
	}

	
	
	
	/**
	 * Ajoute un token � la liste des tokens
	 * 
	 * @param token
	 */
	public void addToken(Token token) {

	}

	
	
	
	/**
	 * Supprime un token de la liste des tokens
	 * 
	 * @param nom
	 */
	public void removeToken(String nom) {

	}

	
	
	
	/**
	 * Cette fonction retourne le PIN de l'utiliateur
	 * 
	 * @return PIN
	 */
	public String getPIN() {
		return PIN;
	}
	
	
	

	/**
	 * Modifie le PIN Utilisateur
	 * 
	 * @param pIN
	 */
	public void setPIN(String pIN) {
		PIN = pIN;
	}

	
	
	
	/**
	 * @param firstTime
	 */
	public void setFirstTime(boolean firstTime) {
		this.firstTime = firstTime;
	}

	
	
	
	/**
	 * cete fonction pr�cise si c'est la premi�re utilisation de l'application
	 * 
	 * @return true si c'est la premi�re utilisation de l'application
	 * @return false sinon
	 */
	public boolean isFirstTime() {
		return firstTime;
	}

	
	
	
	/**
	 * Cette fonction permet de s�rialiser un objt passer en param�tres
	 * 
	 * @param obj
	 * @return byte[]
	 * @throws IOException
	 */
	private byte[] serialize(Object obj) throws IOException {
		ByteArrayOutputStream b = new ByteArrayOutputStream();
		ObjectOutputStream o = new ObjectOutputStream(b);
		o.writeObject(obj);
		return b.toByteArray();
	}

	
	
	
	/**
	 * Cette fonction permet de d�serialiser un contenu de type LocalData
	 * 
	 * @param contenu
	 * @return LocalData
	 * @throws OptionalDataException
	 * @throws ClassNotFoundException
	 * @throws IOException
	 */
	private Object deserialize(byte[] contenu) throws OptionalDataException,
			ClassNotFoundException, IOException {
		ByteArrayInputStream b = new ByteArrayInputStream(contenu);
		ObjectInputStream o = new ObjectInputStream(b);
		return o.readObject();
	}

}
