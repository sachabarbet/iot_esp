#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "led_rgb.h"
#include "i2c_bitbang.h"
#include "detector.h"
#include "timer_manager.h" // Ajout du Timer

// 🛠️ Configuration WiFi & MQTT
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqtt_user = "";
const char* mqtt_password = "";

WiFiClient espClient;
PubSubClient client(espClient);

// 🛠️ Configuration des capteurs
#define LIGHT_THRESHOLD 200  
#define TRIG_PIN 5           
#define ECHO_PIN 18          
#define CYCLE 5000
#define DISTANCE_TRIGGER 150

Led myLed(CYCLE);  
Detector myDetector(TRIG_PIN, ECHO_PIN, CYCLE);  

// 🛠️ Variables pour stocker les valeurs
uint16_t lux = 0;
int distance = -1;
String led_state = "OFF";
String detector_state = "IDLE";
bool led_debounce = false;
String rgb_values = "0,0,0";
String mode = "0";  // Par défaut, on est en mode automatique

// Timers pour éviter les delays bloquants
Timer reconnectTimer(10);   // Vérification MQTT toutes les 5 secondes
Timer mqttSendTimer(45);    // Envoi des données MQTT toutes les 10 secondes

// 🧵 Multi-threading (ESP32 FreeRTOS)
TaskHandle_t MQTTTaskHandle;

// 🔗 Connexion WiFi
void setup_wifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

// 📩 Callback MQTT (Réception des commandes)
void mqttCallback(char* topic, byte* message, unsigned int length) {
    String msg = "";
    for (int i = 0; i < length; i++) {
        msg += (char)message[i];
    }

    Serial.print("📩 MQTT Received: ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(msg);

    if (String(topic) == "esp-led/mode") {
        if (msg == "0") {
            mode = "0";
        } else if (msg == "1") {
            mode = "1";
        }
        Serial.println("🔁 Mode changé en : " + mode);
    } 
    else if (String(topic) == "esp-led/control" && mode == "1") {
        // Parse les valeurs RGB sous forme [R,G,B]
        int r, g, b;
        if (sscanf(msg.c_str(), "[%d,%d,%d]", &r, &g, &b) == 3) {
            setLedState(myLed, LED_ON);
            led_state = "ON";
            setRgbLedColor(r, g, b);
            rgb_values = String(r) + "," + String(g) + "," + String(b);
        } else {
            setLedState(myLed, LED_OFF);
            led_state = "OFF";
            setRgbLedColor(0, 0, 0);
            rgb_values = "0,0,0";
        }
    }
}

// 📡 Reconnexion MQTT 
void reconnect() {
    reconnectTimer.increment();
    if (!client.connected() && reconnectTimer.isExpired()) {
        Serial.println("🔄 Tentative de reconnexion MQTT...");
        if (client.connect("ESP32-Client", mqtt_user, mqtt_password)) {
            Serial.println("✅ MQTT Connected");
            client.subscribe("esp-led/mode");
            client.subscribe("esp-led/control");
            client.setCallback(mqttCallback);
        }
        reconnectTimer.reset();
    }
}

void sendMQTTData() {
    mqttSendTimer.increment();  // 🔥 Assure que le Timer est incrémenté à chaque loop
    if (mqttSendTimer.isExpired()) {  // 🔥 Envoi des données seulement toutes les 5 secondes
        // Vérification si MQTT est bien connecté avant d'envoyer
        if (client.connected()) {
            // 🔄 Mise à jour des valeurs avant l'envoi
            lux = readBH1750();
            myDetector.update();
            distance = myDetector.getDistance();
            detector_state = (myDetector.getState() == Detector::SENDING) ? "SENDING" :
                             (myDetector.getState() == Detector::MEASURING) ? "MEASURING" : "IDLE";

            // 📡 Création du message JSON mis à jour
            String payload = "{";
            payload += "\"espMode\":\"" + mode + "\",";
            payload += "\"ledValues\":\"" + rgb_values + "\",";
            payload += "\"luxValue\":" + String(lux) + ",";
            payload += "\"luxTriggerOn\":0,";
            payload += "\"luxTriggerOff\":" + String(LIGHT_THRESHOLD) + ",";
            payload += "\"distance\":" + String(distance) + ",";
            payload += "\"detectorState\":\"" + detector_state + "\"";
            payload += "}";

            // 📡 Publication sur MQTT
            bool success = client.publish("esp-led/data", payload.c_str(), true);
            if (success) {
                Serial.println("📡 MQTT Publish Success (Toutes les 10s~) : " + payload);
            } else {
                Serial.println("❌ MQTT Publish Failed !");
            }

            mqttSendTimer.reset();  // 🔄 Reinitialization du Timer
        }
    }
}

// 🔄 Mise à jour de la LED en mode AUTO
void updateLedAuto(Led &led, Detector &detector) {
    lux = readBH1750();
    detector.update();
    distance = detector.getDistance();

   // Serial.print("Luminosité : ");
    // Serial.print(lux);
   //  Serial.print(" lux | Distance : ");
   //  Serial.print(distance);
   //  Serial.println(" cm");

    if (lux < LIGHT_THRESHOLD && (distance > 0 && distance < DISTANCE_TRIGGER)) {
        if (!led_debounce) {
            led_debounce = true;
        } else {
            setLedState(led, LED_ON);
            led_state = "ON";
            setRgbLedColor(255, 255, 255);
            rgb_values = "255,255,255";
            led_debounce = false;
        }
    } else {
        if (led_debounce) {
            led_debounce = false;
        } else if (led_state == "ON") {
            setLedState(led, LED_OFF);
            led_state = "OFF";
            setRgbLedColor(0, 0, 0);
            rgb_values = "0,0,0";
        }
    }

    detector_state = (detector.getState() == Detector::SENDING) ? "SENDING" :
                     (detector.getState() == Detector::MEASURING) ? "MEASURING" : "IDLE";
}

// 🔍 Afficher les états
void printStatus() {
    Serial.print("Mode: ");
    Serial.print(mode);
    Serial.print(" | LED State: ");
    Serial.print(led_state);
    Serial.print(" | Detector State: ");
    Serial.print(detector_state);
    Serial.print(" | Timer Counter: ");
    Serial.print(myLed.timer.getCounter());  
    Serial.print(" / ");
    Serial.print(myLed.timer.getThreshold()); 
    Serial.println(" cycles");
}

void setup() {
    Serial.begin(115200);
    i2c_init();
    led_init();
    myDetector.begin();
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    Serial.println("ESP32: Gestion automatique de la LED, capteurs & MQTT");
}

void loop() {
    reconnect();  // Vérifie la connexion MQTT
    client.loop(); // Gère les messages MQTT entrants

    if (mode == "0") {  // Mode AUTO
        updateLedAuto(myLed, myDetector);  // Mise à jour de la LED en mode Auto
    }
    sendMQTTData();  // 🔥 Envoi des données MQTT avec un Timer (toutes les 5 sec)
}
