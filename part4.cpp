#include <WiFi.h>
#include <PubSubClient.h>

// Định nghĩa thông tin MQTT
const char* mqttServer = "YOUR_MQTT_BROKER_IP_OR_HOSTNAME"; // Ví dụ: public.mqtthq.com
const int mqttPort = 1883;
const char* mqttClientName = "esp32_nhom_vju";
// Các Topics
const char* pubTopicSensor = "vju/vju_team_1/sensor";
const char* subTopicLedCmd = "vju/vju_team_1/cmd/led";
const char* pubTopicState = "vju/vju_team_1/state";

WiFiClient espClient;
PubSubClient client(espClient);

// Biến trạng thái (chia sẻ)
// float temp = 25.5; 
// float humi = 60.0;
// int ledState = LOW; 

// Hàm callback khi nhận được message từ Broker
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    // Chuyển payload thành String
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    // Xử lý lệnh điều khiển LED
    if (String(topic) == subTopicLedCmd) {
        if (message == "1") {
            ledState = HIGH;
            digitalWrite(LED_BUILTIN, HIGH);
        } else if (message == "0") {
            ledState = LOW;
            digitalWrite(LED_BUILTIN, LOW);
        }
        
        // Phản hồi trạng thái (Publish)
        client.publish(pubTopicState, ledState == HIGH ? "ON" : "OFF");
    }
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        
        // Last Will and Testament (LWT): Gửi message OFFLINE nếu ngắt kết nối
        if (client.connect(mqttClientName, pubTopicState, 1, true, "OFFLINE")) {
            Serial.println("connected");
            // Subscribe topic lệnh điều khiển
            client.subscribe(subTopicLedCmd);
            
            // Gửi trạng thái ONLINE ban đầu
            client.publish(pubTopicState, "ONLINE", true);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setupMQTT() {
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    // Kết nối Wi-Fi...
    // reconnectMQTT();
}

void loopMQTT() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop(); // Lắng nghe message và duy trì kết nối
}

// Hàm publish cảm biến định kỳ
void publishSensorData() {
    // 1. Đọc cảm biến thực tế
    // readTempAndHumi(&temp, &humi); 
    
    // 2. Tạo payload JSON
    String json = "{";
    json += "\"temp\":" + String(temp) + ",";
    json += "\"humi\":" + String(humi) + ",";
    json += "\"ts\":" + String(millis()) ; // Timestamp
    json += "}";
    
    // 3. Publish (QoS 0, không Retain)
    client.publish(pubTopicSensor, json.c_str());
}
// (Cần gọi publishSensorData() trong loop() với Timer)
