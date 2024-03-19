void loop() {
  // The loop function is empty because the tasks handle everything
}
// Task that get data and sends it to the queue
void sendToQueue(void *parameter) {
  while (1) {
    // Read data from sensor
    struct dataRead currentData;
    sht4.getEvent(&humidity, &temp);
    currentData.Temperature =temp.temperature;
    currentData.Humidity = humidity.relative_humidity;
    currentData.airTemperature = bmp.readTemperature();
    currentData.airPressure = bmp.readPressure()/1000;//kPa
    // Send data to the queue
    if (xQueueSend(Queue, &currentData, portMAX_DELAY) != pdPASS) {
      Serial.println("Failed to send data to the queue");
    }
    // delay for 2 second
     vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }
}

// Task data from the queue
void receiveFromQueue(void *parameter) {
  while (1) {
    // Receive data from the queue
    struct dataRead currentData;
    if (xQueueReceive(Queue, &currentData, portMAX_DELAY) == pdPASS) {
  
    }
    // Process the received data
    if (!client.connected()) {
    reconnect();
    }
    client.loop();
    String data = "{\"data\": {\"Temperature\":"+String(currentData.Temperature)+",\"Humidity\":"+ String(currentData.Humidity)+",\"AirTemperature\":"+ String(currentData.airTemperature)+",\"AirPressure\":"+ String(currentData.airPressure) + "}}";
    Serial.println(data);
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@shadow/data/update", msg);
  }
}
//Reconnect to MQTT Function
void reconnect() {
  while (!client.connected()) {
    Serial.print("Sensor MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
//Callback Msg from MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  // Handle message arrived
}
