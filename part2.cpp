#include <WiFi.h>
#include <WebServer.h>

// Khai báo server trên cổng 80
WebServer server(80);

// Biến trạng thái (chia sẻ với các phần khác)
float temp = 25.5; 
float humi = 60.0;
int ledState = LOW; 
// const int LED_PIN = 2;

// Hàm xử lý GET /api/sensor
void handleApiSensor() {
    // Cấu trúc JSON phản hồi
    String json = "{";
    json += "\"temperature\":" + String(temp) + ",";
    json += "\"humidity\":" + String(humi) + ",";
    json += "\"led_state\":" + String(ledState);
    json += "}";
    
    // Phản hồi JSON
    server.send(200, "application/json", json);
}

// Hàm xử lý POST /api/led?state=0|1
void handleApiLed() {
    if (server.method() != HTTP_POST) {
        server.send(405, "text/plain", "Method Not Allowed");
        return;
    }
    
    // Lấy giá trị 'state' từ query parameter
    if (server.hasArg("state")) {
        int newState = server.arg("state").toInt();
        if (newState == 0 || newState == 1) {
            ledState = newState;
            // Điều khiển LED vật lý
            digitalWrite(LED_BUILTIN, ledState);

            String json = "{\"status\":\"ok\", \"new_state\":" + String(ledState) + "}";
            server.send(200, "application/json", json);
            return;
        }
    }
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid state parameter\"}");
}

// Hàm xử lý trang web mặc định (Trang đơn giản)
void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>ESP32 IoT Status</title>";
    // ... Thêm CSS/JS cơ bản ...
    html += "</head><body>";
    html += "<h1>ESP32 Status</h1>";
    html += "<p>Temperature: " + String(temp) + " &deg;C</p>";
    html += "<p>Humidity: " + String(humi) + " %</p>";
    html += "<p>LED State: " + (ledState ? "ON" : "OFF") + "</p>";
    // Thêm nút (sử dụng form POST hoặc JS)
    html += "<form method='POST' action='/api/led?state=" + String(1 - ledState) + "'>";
    html += "<button type='submit'>" + (ledState ? "Tắt LED" : "Bật LED") + "</button>";
    html += "</form>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
}


void setupWeb() {
    // Kết nối Wi-Fi (Tái sử dụng code của phần 1)
    // WiFi.begin(ssid, pass); 
    // while (WiFi.status() != WL_CONNECTED) { delay(1000); }
    // Serial.println("Connected. IP: " + WiFi.localIP().toString());
    
    // Định nghĩa các Route
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/sensor", HTTP_GET, handleApiSensor);
    server.on("/api/led", HTTP_POST, handleApiLed);

    // Bắt đầu Server
    server.begin();
}

void loopWeb() {
    server.handleClient(); // Xử lý các yêu cầu HTTP đến
    // Tự reconnect Wi-Fi: Nên có một hàm riêng kiểm tra trạng thái và gọi WiFi.reconnect()
}
