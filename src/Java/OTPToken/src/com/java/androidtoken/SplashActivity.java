package com.java.androidtoken;

import android.os.Bundle;
import android.app.Activity;

/**Ecran Splash, premier �cran affich� lors du lancement de l'application
loading des donn�es Utilisateur
sortie: -si premi�re utilisation de l'application, afficher SetPINActivity
        -sinon afficher PINLoginActivity.
*/

public class SplashActivity extends Activity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_splash);
	}

}
