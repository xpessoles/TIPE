#include <WiFi101.h>         // Inclure la bibliothèque WiFi pour la connexion WiFi
#include <Wire.h>            // Inclure la bibliothèque Wire pour la communication I2C
#include <SparkFun_ADXL345.h> // Inclure la bibliothèque SparkFun ADXL345 pour l'accéléromètre ASX00029

#define WIFI_SSID "Nom_de_votre_réseau"    // Remplacez "Nom_de_votre_réseau" par le SSID de votre réseau WiFi
#define WIFI_PASSWORD "Mot_de_passe_de_votre_réseau"  // Remplacez "Mot_de_passe_de_votre_réseau" par le mot de passe de votre réseau WiFi
#define SERVER_IP "Adresse_IP_du_serveur"   // Remplacez "Adresse_IP_du_serveur" par l'adresse IP de votre serveur
#define SERVER_PORT 80    // Port du serveur

ADXL345 accel;           // Créer une instance de l'accéléromètre

char ssid[] = WIFI_SSID;      // SSID du réseau WiFi
char pass[] = WIFI_PASSWORD;  // Mot de passe du réseau WiFi
int status = WL_IDLE_STATUS;  // Statut de la connexion WiFi
WiFiClient client;             // Créer un client WiFi

void setup() {
  Serial.begin(9600);   // Initialiser la communication série à 9600 bauds
  
  if (!accel.begin()) {    // Initialiser l'accéléromètre
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1);    // Boucler indéfiniment
  }
  
  Serial.println("ADXL345 found!");   // Afficher un message de test de l'accéléromètre
  
  if (! WiFi.begin(ssid, pass)) {   // Tenter de se connecter au réseau WiFi
    Serial.println("Erreur de connexion.");   // Afficher un message d'erreur si la connexion échoue
    while (true);   // Boucler indéfiniment
  }
  
  while (WiFi.status() != WL_CONNECTED) {   // Attendre la connexion au réseau WiFi
    delay(500);   // Attendre 500 millisecondes
    Serial.print(".");   // Afficher un point pour montrer que la connexion est en cours
  }
  Serial.println("Connecté");   // Afficher un message lorsque la connexion est établie
}

void loop() {
  float x, y, z;
  
  // Lire les données de l'accéléromètre
  accel.readAccel(&x, &y, &z);
  
  if (client.connect(SERVER_IP, SERVER_PORT)) {   // Se connecter au serveur
    Serial.println("Connexion au serveur...");   // Afficher un message de connexion réussie
    client.print("Accélération X: ");   // Envoyer l'accélération sur l'axe X au serveur
    client.println(x);
    client.print("Accélération Y: ");   // Envoyer l'accélération sur l'axe Y au serveur
    client.println(y);
    client.print("Accélération Z: ");   // Envoyer l'accélération sur l'axe Z au serveur
    client.println(z);
    client.stop();   // Fermer la connexion au serveur
  } else {
    Serial.println("Échec de connexion au serveur.");   // Afficher un message en cas d'échec de connexion au serveur
  }

  delay(1000);   // Attendre 1 seconde avant la prochaine lecture des données
}
