
//fonction lancée par l'appel d'interruption pour le calcul de position ------------------------------------------
void coderRising() {        // cette fonction met a jour la distance parcourue et calcule la période
// attention les signes dépendent de l'ordre de branchement des deux fils du codeur sur les broches de l'Arduino
   if (digitalRead(coderB) == 0) {  // la lecture de la 2eme voie codeur permet de determiner le sens de rotation
       pos--;                       // on décrémente la valeur de position
       sensRotation = -1;         // on indique un sens de rotation négatif
   }
   else {  
       pos++;                    // on incrémente la valeur de position
       sensRotation = 1;         // on indique un sens positif
   }
   
   // calcul de la periode du codeur pour l'indication de la vitesse. 
   frontCodeur = micros();          // mesure du temps absolu depuis le demarrage de la carte (en µs)   
   periodeCodeur = sensRotation*(frontCodeur - precedentFrontCodeur);  // Le signe (+/-) indique le sens de rotation
   precedentFrontCodeur = frontCodeur ;   // pour l'appel d'interruption suivant   
}


// commande du moteur  -------------------------------------------------------------------------------
void commandeMoteur(float pwm)
{
  if (pwm > 0)
    {
      digitalWrite(directionMoteur, LOW);  
      analogWrite(pwmMoteur,int(pwm));      // puissance max pour la valeur 255
    }
  else
    {
      digitalWrite(directionMoteur, HIGH);  
      analogWrite(pwmMoteur,int(-pwm));    // puissance max pour la valeur -255
    } 
}

