package com.java.dataManager;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;

import android.content.Context;

/**
 * Classe permet de cr�er, modifier, supprimer , enregistrer dans un fichier interne du
 * device Android
 * @author ADEGOLOYE Yves
 */
public class IOFileUtils {

	/**
	 * Cette fonction permet de savoir si le fichier pass� en param�tre
	 * existe dans la m�moire interne du device
	 * @param context
	 * @param fileName
	 * @return boolean
	 */
	public static boolean isExistInternalFile(Context context, String fileName) {
		File file = new File(context.getFilesDir(), fileName);
		if (!file.exists()) {
			return false;
		}
		return true;
	}
	
	
	
	/**
	 * Cette fonction cr�e un nouveau fichier avec comme nom , celui pass� en param�tre
	 * @param context
	 * @param fileName
	 */

	public static void createInternalFile(Context context, String fileName) {
		File file = new File(context.getFilesDir(), fileName);
		try {
			file.createNewFile();
		} catch (IOException e) {
		}
	}
	

	
	/**
	 * Cette fonction renvoie le fichier (en m�moire interne du device)
	 * ayant pour nom celui pass� en param�tres.
	 * @param context
	 * @param fileName
	 * @return File
	 */
	public static File getInternalFile(Context context, String fileName) {
		File file = new File(context.getFilesDir() + "/" + fileName);
		return file;
	}
	
	
	/**
	 * Cette fonction supprime de la m�moire interne le fichier ayant pour nom
	 * celui pass� en param�tres
	 * @param context
	 * @param fileName
	 * @return boolean
	 */

	public static boolean deleteFile(Context context, String fileName) {
		File dir = context.getFilesDir();
		File file = new File(dir, fileName);
		boolean deleted = file.delete();
		return deleted;
	}
	
	
	/**
	 * Cette fonction enregistre le contenu pass� en param�tre dans le fichier ,
	 * @param context
	 * @param fileName
	 * @param contenu en byte[]
	 */

	public static void saveToInternalFile(Context context, String fileName,byte[] contenu) {
		FileOutputStream outputStream;
		try {
			outputStream = context.openFileOutput(fileName,Context.MODE_PRIVATE);
			outputStream.write(contenu);
			outputStream.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	/**
	 * Cette fonction lire le fichier pass� en param�tre ,
	 * @param context
	 * @param fileName
	 * @param contenu en byte[]
	 */

	public static byte[] readFromInternalFile(Context context, String fileName) {
		FileInputStream inputStream;
		byte[] content = null;
		try {
			inputStream = context.openFileInput( fileName);
			content = new byte[inputStream.available()];
            inputStream.read(content);
            inputStream.close();
		} catch (Exception e) {
		}
		return content;
	}
	
	
	/**
	 * Cette fonction supprime le contenu le fichier ayant pour nom
	 * celui pass� en param�tres
	 * @param context
	 * @param fileName
	 * @return boolean
	 */

	public static void clearFile(Context context, String fileName) {
		File dir = context.getFilesDir();
		File file = new File(dir, fileName);
		PrintWriter writer;
		try {
			writer = new PrintWriter(file);
			writer.print("");
			writer.close();
		} catch (FileNotFoundException e) {
		}

	}

}
