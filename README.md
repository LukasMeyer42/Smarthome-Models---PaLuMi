# Projektidee: Automatisches Hausbelüftungssystem

## Beschreibung
Das automatische Hausbelüftungssystem ist ein Projekt, das darauf abzielt, die Luftqualität in einem Haus durch die automatische Steuerung von Fenstern zu verbessern. Das System verwendet verschiedene Sensoren, um Umgebungsparameter wie Temperatur, Luftfeuchtigkeit und Gaskonzentration zu überwachen. Basierend auf den gemessenen Werten steuert ein ESP32-Server die Öffnung und Schließung von Fenstern, um die Luftqualität im Haus zu optimieren.

## Funktionsweise
- Das System ist mit einem DHT11-Temperatur- und Luftfeuchtigkeitssensor sowie einem Gassensor mit 4 Pins (DO, AO, VCC, GND) ausgestattet.
- Ein ESP32-Server dient als Steuerzentrale des Systems und kommuniziert mit einem MQTT-Broker.
- Sobald die Sensoren einen bestimmten Schwellenwert erreichen, öffnet das System automatisch ein Fenster im Haus, um Frischluft einzulassen.
- Ein Servomotor steuert die Öffnung und Schließung des Fensters basierend auf den vom ESP32 empfangenen Befehlen.
- Das Fenster schließt automatisch, wenn die gemessenen Werte wieder unter den Schwellenwert fallen.

## Benötigte Komponenten
- DHT11-Temperatur- und Luftfeuchtigkeitssensor
- Gassensor mit 4 Pins: Digital Out (DO), Analog Out (AO), VCC, GND
- ESP32-Server
- MQTT-Broker

## Einrichtung
1. Verbinden Sie die Sensoren gemäß den Anweisungen mit dem ESP32.
2. Laden Sie den entsprechenden Code auf den ESP32 hoch.
3. Konfigurieren Sie den ESP32 so, dass er sich mit dem MQTT-Broker verbindet.
4. Starten Sie das System und überwachen Sie die Sensordaten über den MQTT-Broker.
