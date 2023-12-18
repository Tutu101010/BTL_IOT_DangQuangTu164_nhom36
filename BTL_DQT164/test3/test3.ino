#include<WiFi.h>
#include <PubSubClient.h>

const int LIGHT_SENSOR_PIN = 17;
const int LEDPIN = 19;
  //Khai báo loại cảm biến
const char* ledTopic = "led";  // Chủ đề MQTT cho điều khiển đèn LED
const char* sensorTopic = "sensor";  // Chủ đề MQTT cho dữ liệu cảm biến


const char *mqtt_broker = "172.20.10.3";
const int mqtt_port = 1883;
WiFiClient wifiClient;
PubSubClient client(wifiClient);


//char* ssid = "Dang Quang Tu 5G";
//const char* password = "88888888@";
char* ssid = "Dang Quang Tu";
const char* password = "11111111";

void setup() {
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float l = analogRead(LIGHT_SENSOR_PIN);
  
  String sensorMessage = "{\"light\": " + String(l) + "}";
  client.publish(sensorTopic, sensorMessage.c_str());

  Serial.print("% Light Value: ");
  Serial.println(l);

  Serial.println();                //Xuống hàng
  delay(500);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

   if (strcmp(topic, ledTopic) == 0) {
    // Xử lý thông điệp điều khiển đèn LED
    if (message.equals("ON")) {
      digitalWrite(LEDPIN, HIGH);
      Serial.println("LED turned on");
    } else if (message.equals("OFF")) {
      digitalWrite(LEDPIN, LOW);
      Serial.println("LED turned off");
    }
  }

}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(ledTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}