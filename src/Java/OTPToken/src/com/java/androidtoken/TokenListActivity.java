package com.java.androidtoken;

import android.os.Bundle;
import android.app.ListActivity;

/**Ecran d'accueil regroupant la liste des OTP cr��
Sortie -si "Bouton menu Android AddToken" clik�, afficher AddTokenActivity
       -si "Bouton menu Android RemoveToken" click� , afficher RemoveTokenActivity
       -sinon si "menu cancel Bouton" click�, �teindre l'application
*/

public class TokenListActivity extends ListActivity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_token_list);
	}

}
