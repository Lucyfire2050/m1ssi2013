package com.java.androidtoken;

import android.app.Activity;
import android.os.Bundle;

/**Ecran d'initialisation ou du mis � jour du PIN
*Ecran affich� soit � la premi�re utilisation de l'application
*soit lorsque l'utilisateur d�sire modifier son PIN depuis le menu Android.
*Sortie: afficher l'�cran pr�c�dent
*@author ADEGOLOYE Yves
*/

public class SetPINActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_set_pin);
	}

}
