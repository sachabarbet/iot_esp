#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Définition des broches
#define SERVO_PIN 13  // Broche pour le servo-moteur
#define I2C_SDA 21    // Broche SDA pour le capteur
#define I2C_SCL 22    // Broche SCL pour le capteur

// Configuration WiFi
const char* ssid = "Tilaskarot";
const char* password = "vn635muss4689p8";

// Configuration MQTT
const char* mqtt_server = "10.250.192.233";
const int mqtt_port = 1883;
const char* mqtt_user = "esp_user";
const char* mqtt_password = "ynov1234";
const char* mqtt_client_id = "ESP32_VOLET";

// Topics MQTT
const char* topic_data = "esp-shutter/data";
const char* topic_control = "esp-shutter/control";
const char* topic_mode = "esp-shutter/mode";

// Configuration du servo
#define SERVO_MIN_US 500    // Durée d'impulsion pour 0°
#define SERVO_MAX_US 2400   // Durée d'impulsion pour 180°

// Seuils de luminosité
#define SEUIL_LUMINOSITE_HAUT 100.0  // Lux
#define SEUIL_LUMINOSITE_BAS 50.0    // Lux

// Machine à états principale
enum Etat {
  INIT,           // Initialisation
  WIFI_CONNECT,   // Connexion WiFi
  MQTT_CONNECT,   // Connexion MQTT
  NORMAL,         // Fonctionnement normal
  MOUVEMENT,      // Mouvement du servo
  PUBLICATION     // Publication des données
};

// Modes de fonctionnement
enum Mode {
  AUTO = 0,       // Mode automatique
  MANUEL = 1      // Mode manuel
};

// Variables globales
BH1750 capteurLumiere;
Etat etatCourant = INIT;
Mode modeActuel = AUTO;
int angleActuel = 0;
int angleCible = 0;
int positionPct = 0;
uint32_t compteurEtat = 0;
uint32_t compteurMouvement = 0;
uint32_t compteurPublication = 0;
uint32_t compteurWiFiReconnect = 0;
uint32_t compteurLuminosite = 0;
bool capteurOk = false;
bool wifiConnecte = false;
bool mqttConnecte = false;
bool changementEtat = false;  // Flag pour suivre les changements d'état

// Objets WiFi et MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Fonction pour générer l'impulsion du servo
void positionServo(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  
  int duree = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(duree);
  digitalWrite(SERVO_PIN, LOW);
}

// Publication des données via MQTT
void publierDonnees() {
  if (!mqttConnecte) return;
  
  float lux = capteurOk ? capteurLumiere.readLightLevel() : -1;
  String message = "{\"position\":" + String(positionPct) + 
                  ",\"mode\":" + String(modeActuel) + 
                  ",\"luminosite\":" + String(lux) + "}";
  
  mqttClient.publish(topic_data, message.c_str());
  Serial.println("Données publiées: " + message);
}

// Callback MQTT
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  // Conversion du payload en string
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  
  Serial.print("Message MQTT reçu [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);
  
  // Traitement selon le topic
  if (strcmp(topic, topic_mode) == 0) {
    // Changement de mode
    int mode = atoi(message);
    if (mode == 0 || mode == 1) {
      modeActuel = static_cast<Mode>(mode);
      Serial.print("Mode changé: ");
      Serial.println(modeActuel == AUTO ? "AUTO" : "MANUEL");
      
      // Marquer le changement d'état sans publication immédiate
      changementEtat = true;
    }
  }
  else if (strcmp(topic, topic_control) == 0 && modeActuel == MANUEL) {
    // Contrôle manuel de la position
    int position = atoi(message);
    if (position >= 0 && position <= 100) {
      Serial.print("Position demandée: ");
      Serial.println(position);
      
      positionPct = position;
      angleCible = map(position, 0, 100, 0, 180);
      
      // Passage en mode mouvement sans publication immédiate
      etatCourant = MOUVEMENT;
      compteurMouvement = 0;
      changementEtat = true;
    }
  }
}

// Initialisation du WiFi avec timeout progressif
bool setupWiFi(bool forceReconnect = false) {
  if (WiFi.status() == WL_CONNECTED && !forceReconnect) {
    if (!wifiConnecte) {
      Serial.println("WiFi reconnecté");
      wifiConnecte = true;
    }
    return true;
  }
  
  // Si déconnecté et temps écoulé depuis dernière tentative
  if (compteurWiFiReconnect == 0 || forceReconnect) {
    Serial.println("\nConnexion WiFi...");
    // Déconnexion préalable
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    // Configuration
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    // Paramètres avancés pour améliorer la connexion
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    // Attente initiale
    for (int i = 0; i < 50; i++) {
      if (WiFi.status() == WL_CONNECTED) break;
      Serial.print(".");
      delay(100);
    }
  }
  
  // Vérification de l'état
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connecté");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
    wifiConnecte = true;
    compteurWiFiReconnect = 0;
    return true;
  } else {
    if (compteurWiFiReconnect % 25 == 0) { // Toutes les 0.5 seconde environ
      Serial.print(".");
    }
    
    compteurWiFiReconnect++;
    if (compteurWiFiReconnect >= 500) { // 10 secondes
      compteurWiFiReconnect = 0; // Reset pour nouvelle tentative
    }
    
    wifiConnecte = false;
    return false;
  }
}

// Connexion au serveur MQTT
bool connectMQTT() {
  if (mqttClient.connected()) {
    mqttConnecte = true;
    return true;
  }
  
  if (compteurEtat % 250 == 0) { // Tentative toutes les 5 secondes
    Serial.print("Connexion MQTT...");
    
    if (mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("connecté");
      
      // Souscription aux topics
      mqttClient.subscribe(topic_control);
      mqttClient.subscribe(topic_mode);
      
      // Message de connexion (uniquement à la connexion)
      mqttClient.publish(topic_data, "{\"status\":\"connected\"}");
      
      mqttConnecte = true;
      return true;
    } else {
      Serial.print("échec, rc=");
      Serial.println(mqttClient.state());
      mqttConnecte = false;
      return false;
    }
  }
  
  return false;
}

void setup() {
  // Initialisation série
  Serial.begin(115200);
  
  Serial.println("\n=== Système volet roulant ===");
  
  // Configuration GPIO
  pinMode(SERVO_PIN, OUTPUT);
  
  // Initialisation I2C et capteur
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  if (capteurLumiere.begin()) {
    capteurOk = true;
    Serial.println("Capteur BH1750 initialisé");
    
    // Lecture test
    float lux = capteurLumiere.readLightLevel();
    Serial.print("Luminosité initiale: ");
    Serial.print(lux);
    Serial.println(" lux");
  } else {
    capteurOk = false;
    Serial.println("ATTENTION: Capteur BH1750 non détecté");
  }
  
  // Configuration MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callbackMQTT);
  
  // Test initial du servo
  Serial.println("Test du servo:");
  
  Serial.println("Position 0° (fermé)");
  for (int i = 0; i < 50; i++) {
    positionServo(0);
    delay(20);
  }
  
  Serial.println("Position 180° (ouvert)");
  for (int i = 0; i < 50; i++) {
    positionServo(180);
    delay(20);
  }
  
  Serial.println("Position 0° (fermé)");
  for (int i = 0; i < 50; i++) {
    positionServo(0);
    delay(20);
  }
  
  // État initial
  angleActuel = 0;
  angleCible = 0;
  positionPct = 0;
  etatCourant = WIFI_CONNECT;
  
  Serial.println("Initialisation terminée");
}

void loop() {
  // Incrémentation du compteur d'état
  compteurEtat++;
  
  // Machine à états principale
  switch (etatCourant) {
    case WIFI_CONNECT:
      // Tentative de connexion WiFi
      if (setupWiFi()) {
        etatCourant = MQTT_CONNECT;
      }
      break;
      
    case MQTT_CONNECT:
      // Vérification préalable du WiFi
      if (!wifiConnecte) {
        etatCourant = WIFI_CONNECT;
        break;
      }
      
      // Tentative de connexion MQTT
      if (connectMQTT()) {
        etatCourant = NORMAL;
      }
      break;
      
    case NORMAL:
      // Vérification des connexions
      if (!wifiConnecte) {
        etatCourant = WIFI_CONNECT;
        break;
      }
      
      if (!mqttConnecte) {
        etatCourant = MQTT_CONNECT;
        break;
      }
      
      // Traitement des messages MQTT
      mqttClient.loop();
      
      // Mode AUTO: vérification de la luminosité
      if (modeActuel == AUTO && capteurOk) {
        compteurLuminosite++;
        
        if (compteurLuminosite >= 100) { // 2 secondes à 50Hz
          float lux = capteurLumiere.readLightLevel();
          Serial.print("Luminosité: ");
          Serial.print(lux);
          Serial.println(" lux");
          
          if (lux > SEUIL_LUMINOSITE_HAUT && angleActuel < 90) {
            Serial.println("Luminosité élevée -> Ouverture");
            angleCible = 180;
            etatCourant = MOUVEMENT;
            compteurMouvement = 0;
            changementEtat = true;
          }
          else if (lux < SEUIL_LUMINOSITE_BAS && angleActuel > 90) {
            Serial.println("Luminosité faible -> Fermeture");
            angleCible = 0;
            etatCourant = MOUVEMENT;
            compteurMouvement = 0;
            changementEtat = true;
          }
          
          compteurLuminosite = 0;
        }
      }
      
      // Publication périodique (uniquement toutes les 10 secondes)
      compteurPublication++;
      if (compteurPublication >= 500) { // 10 secondes à 50Hz
        etatCourant = PUBLICATION;
        compteurPublication = 0;
      }
      break;
      
    case MOUVEMENT:
      // Mouvement du servo
      angleActuel = angleCible; // Mouvement instantané pour simplifier
      positionPct = map(angleActuel, 0, 180, 0, 100);
      
      compteurMouvement++;
      if (compteurMouvement >= 50) { // 1 seconde
        Serial.print("Mouvement terminé: ");
        Serial.print(angleActuel);
        Serial.print("° (");
        Serial.print(positionPct);
        Serial.println("%)");
        
        // Retour à l'état normal sans publication immédiate
        etatCourant = NORMAL;
      }
      break;
      
    case PUBLICATION:
      // Publication des données
      publierDonnees();
      etatCourant = NORMAL;
      break;
      
    default:
      // État invalide, retour à l'initialisation
      etatCourant = WIFI_CONNECT;
      break;
  }
  
  // Impulsion servo
  positionServo(angleActuel);
  
  // Délai pour obtenir 50Hz
  delay(20);
}