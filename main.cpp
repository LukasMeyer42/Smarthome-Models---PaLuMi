#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// Replace with your network credentials
const char* ssid = "SensorNet";
const char* password = ""; // Füge dein WLAN-Passwort hier ein

// Replace with your MQTT broker details
const char* mqtt_server = "192.168.1.111";
const char* mqtt_topic_temperature = "esp32/dht/temperature";
const char* mqtt_topic_humidity = "esp32/dht/humidity";
const char* mqtt_topic_gas = "esp32/gas";
const char* mqtt_topic_schloss = "esp32/output";

// DHT sensor setup
#define DHTPIN 5
#define DHTTYPE DHT11
#define GASPIN A12

static const int servoPin = 32;
Servo myservo; 
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

bool schloss_geoeffnet = false; // Variable zur Speicherung des Schlossstatus

void callback(char* topic, byte* payload, unsigned int length); // Vorwärtsdeklaration der callback-Funktion

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  myservo.attach(servoPin);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); // Setze die Callback-Funktion
  dht.begin();
  client.subscribe(mqtt_topic_schloss); // Abonnement auf das Schlossthema
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);
  
  // Überprüfe, ob das empfangene Thema das Schlossthema ist
  if (strcmp(topic, mqtt_topic_schloss) == 0) {
    int value = atoi(message);
    // Ausgabe abhängig vom empfangenen Wert
    if (value == 1) {
      Serial.println("Schloss geöffnet");
      myservo.write(180);
      schloss_geoeffnet = true; // Schlossstatus aktualisieren
    } else if (value == 0) {
      Serial.println("Schloss geschlossen");
      myservo.write(90);
      schloss_geoeffnet = false; // Schlossstatus aktualisieren
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_schloss); // Abonnement erneuern nach Verbindungsherstellung
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int sensorValue = analogRead(GASPIN);

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (isnan(sensorValue)) {
    Serial.println("Failed to read from gas sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Gas: ");
  Serial.println(sensorValue);

  // Wenn der Sensorwert über einem Schwellenwert liegt und das Schloss geschlossen ist, öffne das Schloss
  if ((temperature > 25 || humidity > 80 || sensorValue > 200) && !schloss_geoeffnet) {
    client.publish(mqtt_topic_schloss, "1", true);
    Serial.println("Schloss wird geöffnet AUTO");
  }

  // Wenn das Schloss geöffnet ist und die Bedingungen nicht mehr erfüllt sind, schließe das Schloss
  if ((temperature <= 25 && humidity <= 80) && schloss_geoeffnet) {
    client.publish(mqtt_topic_schloss, "0", true);
    Serial.println("Schloss wird geschlossen AUTO ");
  }

  if (!isnan(temperature) && !isnan(humidity) && !isnan(sensorValue)) {
    // Publish sensor data to MQTT topics
    client.publish(mqtt_topic_temperature, String(temperature).c_str(), true);
    client.publish(mqtt_topic_humidity, String(humidity).c_str(), true);
    client.publish(mqtt_topic_gas, String(sensorValue).c_str(), true);

  }
  // Sende Daten alle 2 Sekunden
  delay(2000);
}
