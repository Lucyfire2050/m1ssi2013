package com.java.androidtoken;


import android.app.Activity;
import android.os.Bundle;


/**Ecran d'Ajout d'un Token
 Ecran affich� lorsque l'utilisateur d�sire ajouter un Token depuis le menu Android
 Sortie: - si "Bouton enregistrement du Token" click�, afficher TokenListActivity
         - sinon si "menu cancel Bouton" click�, afficher l'�cran pr�c�dent
*/       


public class RemoveTokenActivity extends Activity {

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_token_add);			
	}

}
