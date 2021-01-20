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

//#define testModeLabview    // activer cette fonction permet d'exploiter des entrées sorties réalisées avec la carte NI6008 et Labview


//**************************************************************************************************************
// déclaration des variables ***********************************************************************************

// variables pour la commande
  int compte = 0;  // compte les cycles pour affichage discontinu sur le port série 
  int val_potentio = 0;                     // valeur de l'entrée potentiomètre pour la commande --> non exploitée dans ce programme
  float consigne = 0;    // variable de la position souhaitée moteur ; le signe ( + / - )  change le sens de rotation 
  #define periodePID 500                // 500 (en microsecondes) = période d'échantillonnage
  // attention supprimer les envois vers le port série si valeur inférieure à 10000 microsecondes ; ou alors n'envoyer que ponctuellement.
  float mes = 0 , mesprec = 0 ;   // pour mesure de la période d'échantillonnage
  float periodeechantmesuree = 0; // pour mesure de la période d'échantillonnage

/*  Assignation des voies de la carte arduino  */
  int pwmMoteur = 9, directionMoteur = 11 ;
  int coderA = 2, coderB = 4;                // une entrée du codeur est sur une entrée d'interruption (2 ou 3)
  int voie_potentio = 0;                      // voie d'entrée analogique du potentiometre de commande --> non exploitée dans ce programme
  int led = 4;                           // sortie numérique (0 ou 1) utilisable pour tests à l'oscillo
  int sortiesignauxtest1 = 5 ;             // sortie PWM utilisable pour exploitation signaux avec Labview
  int sortiesignauxtest2 = 6 ;            // sortie PWM utilisable pour exploitation signaux avec Labview
  // attention : les sorties utilisées sont des sorties PWM ; donc à filtrer (condo : 470 mf) pour obtenir un signal continu.
  
/*  Variables du codeur incremental  */
// Regle : Les variables globales dont la valeur peut etre modifiée 
// par un appel d'interruption doivent être déclarées en volatile
  volatile float frontCodeur= micros();
  volatile float precedentFrontCodeur = micros();
  volatile float periodeCodeur = 1000000 ; //1seconde
  volatile float pos = 0;
  volatile float sensRotation = -1;
  
  long tempsPrecedent =0;
  long temps =0;
  int periode = 0; // pour affichage période d'échantillonnage
  float vitesse =0;
  float vitesse_reducteur_tr_s = 0;
  float vitesse_moteur_rad_s = 0;  
  float vitesse_moteur_tours_s = 0;
  float pi = 3.141597;
      
// génération des créneaux
  int consigne_inferieure = 53;
  int consigne_superieure = 106;
  int periode_creneau = 2000; //en millisecondes
  long mesure_temps_creneau = millis();
  long mesure_temps_creneau_precedent = millis();
  int temps_creneau = 0;

// *************************************************************************************************************************
// partie "initialisation" du programme exécutée une seule fois à la mise sous tension de l'Arduino*************************
void setup() {
  attachInterrupt(0, coderRising, RISING);  
  // active l'interruption 0 (qui correspond à la pin 2) 
  // chaque front montant appelle la fontion coderRising()

  Serial.begin(115200);   // active la transmission d'informations par le port série

  pinMode(directionMoteur, OUTPUT);   
  pinMode(pwmMoteur, OUTPUT);   
  pinMode(coderA, INPUT);  
  pinMode(coderB, INPUT);  
  commandeMoteur(0);                  // stoppe le moteur
  
  pinMode(sortiesignauxtest1, OUTPUT);  
  pinMode(sortiesignauxtest1, OUTPUT);  
  
}

//**********************************************************************************************************
// programme principal : boucle "loop" exécutée indéfiniment >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void loop() {
  // digitalWrite(led,LOW);              // permet de visualiser temps d'attente à l'oscilloscope
  /* attend la prochaine période déchantillonnage  */
  do {
      temps = micros();                // temps absolu depuis le demarrage de la carte (en µs)
      periode = temps - tempsPrecedent;
    } while (periode < periodePID ) ;
  tempsPrecedent = temps ;
  // digitalWrite(led,HIGH);             // permet de visualiser la fin du temps d'attente
  
  // calcul de la vitesse : -----------------------------------------------------------------------------------------
  //vitesse = 1000000/periodeCodeur; // cas où il n'y a pas de filtrage
  vitesse = 0.8*vitesse + 0.2* 1000000/periodeCodeur;   //(cas du filtrage de la vitesse)
  /*   On peut filter la mesure de vitesse en gardant 80% de la vitesse precedente , 
  et en ajoutant 20% de la vitesse mesurée ; attention, il y aura un léger retard sur la réponse affichée.
  */ 
  periodeCodeur*=1.01;        // augmente la periode de 1% à chaque cycle  ;
                              // pour la rendre très grande dans le cas où le moteur ne tourne plus
                              // et ainsi afficher une vitesse nulle
  vitesse_reducteur_tr_s = vitesse/(3*53); //codeur 3 points par tour et réduction 53 du réducteur.
  vitesse_moteur_tours_s = vitesse/3;   //codeur 3 points par tour 
  vitesse_moteur_rad_s = 2*pi*vitesse/3;   //codeur 3 points par tour 
  
  // commande de créneaux de vitesse en Boucle ouverte ---------------------------------------------------------------
  mesure_temps_creneau = millis();
  temps_creneau = mesure_temps_creneau - mesure_temps_creneau_precedent;
  if (temps_creneau < periode_creneau / 2) {
    consigne = consigne_inferieure;
  }
  else if (temps_creneau < periode_creneau) {
    consigne = consigne_superieure;
  }
  else {
    mesure_temps_creneau_precedent = mesure_temps_creneau ;
  }
  commandeMoteur(consigne); // utilisation de la fonction de commande du moteur (onglet "fonctions")

  //affichage des valeurs sur le port série >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //   attention : l'affichage à travers le port série consomme une milliseconde de temps par caractère 
  //   cela risque d'être supérieur à la période fixée pour le calcul du PID et rendre aléatoire le comportement 
  #ifdef testModePortserie
      if (compte==10) {                 // affichage tous les 10 cycles (0,5 ms x 10 = 5 ms --> 500 Hz)
        Serial.print(consigne); // affiche la consigne
        Serial.print(" ");
        //Serial.println(periode);//pour tests seulement : : période mesurée sans envoi des données sur port série : 500 microsecondes
        //Serial.println(pos*6.44);  // affiche la position mesurée
        Serial.println(vitesse_moteur_rad_s); 
        compte=0;
      }  
  #endif
  compte = compte + 1 ;     // incrémentation du compteur de cycles
  
  
  //communication avec Labview grâce aux entrées-sorties d'arduino    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  
  #ifdef testModeLabview
    // affichage sur les sorties PWM : sorties digitales sur 8 bits = 256 points 
    // affichage de la consigne : >>>>>>>>>>>>>>>>>

    analogWrite(sortiesignauxtest1,constrain(val_potentio/4,0,255)) ;   // envoi pour visu labview
    // affichage de la position ou de la vitesse : >>>>>>>>>>>>>>>>>>>>>>>>>>
    analogWrite(sortiesignauxtest2,constrain((pos/4)+128,0,255)) ;   // envoi pour visu labview
    //analogWrite(sortiesignauxtest2,constrain(vitesse+128,0,255)) ;   // envoi pour visu labview
    
    // mesure de la période d'échantillonnage réelle :
    mes = micros();   
    periodeechantmesuree = mes-mesprec ; // en microsecondes
    float affiche;
    affiche = 5*(periodeechantmesuree*255)/10000; // pour atteindre 255 si 10 millisecondes de période 
    //analogWrite(sortiesignauxtest2,constrain(affiche,0,255)) ;   // envoi pour visu labview
    //Serial.println(round(periodeechantmesuree),DEC);
    mesprec = mes ;
    // periode minimale possible mesurée à 6,3 millisecondes (avec une seule sortie de valeur sur le port série !!)
  #endif
  
}




