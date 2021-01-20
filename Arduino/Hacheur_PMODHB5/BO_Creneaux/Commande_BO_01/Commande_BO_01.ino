/*
programme pour piloter le motoreducteur PMT1-53 de Lextronic ; avec la carte PmodHB5.
connectique arduino :                            connectique de la carte PmodHB5 : 
       A0 --> potentiomètre
       D11 -->DIR                               <--  broche 1 (la plus éloignée du connecteur de puissance) 
       D9 --> EN(pwm)                           <--  broche 2 de la carte PmodHB5)
       D4 --> SB(codeur voieB)                  <--  broche 3 de la carte PmodHB5)
       D2 = interruption 0 --> SA(codeur voieA) <--  broche 4 de la carte PmodHB5)
       masse                                    <--  broche 5 de la carte PmodHB5 
       5V                                       <--  broche 6 de la carte PmodHB5 
                                                broche + du connecteur de puissance reliée au Vin de la carte arduino et au + du contacteur d'alim 12V
                                                broche - du connecteur de puissance reliée à la masse de la carte arduino et au - de l'alim 12V 
*/


#define testModePortserie  // activer cette fonction permet de générer des affichages sur la fenêtre du port série, 
// et en conséquence de permettre l'affichage dans un programme qui lit le port série ;
// mais cela consomme du temps de calcul exagérément. (activer en supprimant //)
// l'envoi est calibré pour être lu par le programme python : "PC-recoit-d_arduino-2valeurs-0_1000.py"
// "consigne_position ; espace ; mesure_position"



//**************************************************************************************************************
// déclaration des variables ***********************************************************************************

// variables pour la commande
  int compte = 0;  // compte les cycles pour affichage discontinu sur le port série 
  float consigne = 0;    // variable de la position souhaitée moteur ; le signe ( + / - )  change le sens de rotation 
  

/*  Assignation des voies de la carte arduino  */
  int pwmMoteur = 9, directionMoteur = 11 ;
  int coderA = 2, coderB = 4;                // une entrée du codeur est sur une entrée d'interruption (2 ou 3)
 

  volatile float pos = 0;
  volatile float sensRotation = -1;



// *************************************************************************************************************************
// partie "initialisation" du programme exécutée une seule fois à la mise sous tension de l'Arduino*************************
void setup() {
 
  Serial.begin(115200);   // active la transmission d'informations par le port série
  pinMode(directionMoteur, OUTPUT);   
  pinMode(pwmMoteur, OUTPUT);   
   
}


// programme principal : boucle "loop" exécutée indéfiniment
void loop() {
 consigne = 200;
 if (consigne > 0)
    {
      digitalWrite(directionMoteur, LOW);  
      analogWrite(pwmMoteur,int(consigne));      // puissance max pour la valeur 255
    }
  else
    {
      digitalWrite(directionMoteur, HIGH);  
      analogWrite(pwmMoteur,int(-consigne));    // puissance max pour la valeur -255
    } 
  
 
 

  //affichage des valeurs sur le port série 
  //   attention : l'affichage à travers le port série consomme une milliseconde de temps par caractère 
  //   cela risque d'être supérieur à la période fixée pour le calcul du PID et rendre aléatoire le comportement 
  #ifdef testModePortserie
      if (compte==10) {                 // affichage tous les 10 cycles (0,5 ms x 10 = 5 ms --> 500 Hz)
        Serial.print(consigne); // affiche la consigne
        Serial.print(" ");
               compte=0;
      }  
  #endif
  compte = compte + 1 ;     // incrémentation du compteur de cycles
  
  
  
  
}
