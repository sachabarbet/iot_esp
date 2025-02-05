#include <WiFiClientSecure.h>
#include <LittleFS.h>
#include <PubSubClient.h>

const char* ssid = "ssid";
const char* password = "pass";
const char* mqtt_server = "serv";
const char* mqtt_user = "user";
const char* mqtt_password = "pass";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void loadCertificate() {
    File cert = LittleFS.open("/mosquitto.der", "r");  // Use SPIFFS.open() if using SPIFFS
    if (!cert) {
        Serial.println("Failed to open cert file");
        return;
    }

    size_t size = cert.size();
    char* certData = (char*)malloc(size + 1);
    if (!certData) {
        Serial.println("Memory allocation failed");
        return;
    }

    cert.readBytes(certData, size);
    certData[size] = '\0';  // Null-terminate the buffer

    espClient.setCACert(certData);  // Pass it as a C-string

    free(certData);
    cert.close();
}

void setup_wifi() {
    WiFi.begin(ssid, password);
    while (!WiFi.isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]); // Convert byte to char
    }
    Serial.println();
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect("SB_ESP-CLIENT", mqtt_user, mqtt_password)) {
            Serial.println("MQTT Connected");
            client.subscribe("test/topic");
            client.setCallback(mqttCallback);
            client.publish("test/topic", "Hello from ESP32");
        } else {
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(9600);
    setup_wifi();
    loadCertificate();
    client.setServer(mqtt_server, 1883);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}
