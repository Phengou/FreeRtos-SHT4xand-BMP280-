//Name CHEA pheng ou ID 6672054521
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "Adafruit_SHT4x.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <WiFi.h>
#include <PubSubClient.h>
/*********************WiFi and MQTT Broker****************************/
const char* ssid = "IOTs";
const char* password = "24681012";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "36a41209-c7d1-4ac9-ac7c-effa633827fa";//ClientID
const char* mqtt_username = "UsDx6T2CmuASrwXGHvshuktei7BiZ9bY";//Token
const char* mqtt_password = "4GCD1qYd2PTHtdqgnqJK1ZS8kPcvSn6S";//Secret
char msg[200];
WiFiClient espClient;
PubSubClient client(espClient);
/*******************Sensor**********************/
Adafruit_BMP280 bmp; // Define BMP280 Sensor
Adafruit_SHT4x sht4 = Adafruit_SHT4x(); //Define SHT4x Sensor
sensors_event_t humidity, temp;
/**************Structure of Queue**********/
// Define a struct
struct dataRead {
  float Temperature;
  float Humidity;
  float airPressure;
  float airTemperature;
};
QueueHandle_t Queue; //Define QueueHandle_t

void setup() {
  Serial.begin(9600);
  Wire.begin(41, 40);
  if (bmp.begin(0x76)) { // prepare BMP280 sensor
    Serial.println("BMP280 sensor ready");
  }
  if ( sht4.begin()) { // prepare BMP280 sensor
    Serial.println("SHT4x sensor ready");
  }
    Queue = xQueueCreate(100,sizeof(struct dataRead));

    if (Queue != NULL) {
      Serial.println("Error creating the queue");
    }
    //Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    //connect client to MQTT Broker
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    if(!client.connected()){
      Serial.println("Cucumber's not connected to MQTT Broker");
      client.connect(mqtt_Client, mqtt_username, mqtt_password);
    }
    // Create task that consumes the queue.
    xTaskCreate(sendToQueue, "Sender", 2048, NULL, 2, NULL);
    xTaskCreate(receiveFromQueue, "Receiver", 2048, NULL, 1, NULL);
}
