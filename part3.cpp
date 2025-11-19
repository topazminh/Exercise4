#include <WiFi.h>
#include <WebSocketsServer.h> // Thay thế cho WebServer.h nếu muốn dùng Async

// Khai báo WebSocket Server trên cổng 81 (thường dùng)
WebSocketsServer webSocket = WebSocketsServer(81); 

// Biến trạng thái (chia sẻ)
// float temp = 25.5; 
// float humi = 60.0;
// int ledState = LOW; 

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            
            // Gửi trạng thái ban đầu khi Client kết nối
            webSocket.sendTXT(num, "{\"log\":\"Connected to ESP32\"}");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] Get Text: %s\n", num, payload);
            
            // Phân tích JSON từ client (lệnh điều khiển LED)
            // Ví dụ: {"cmd":"led", "state":1}
            String message = (char*)payload;
            if (message.indexOf("{\"cmd\":\"led\"") != -1) {
                // Giả sử đã parse JSON và lấy được newState
                int newState = message.indexOf("\"state\":1") != -1 ? 1 : 0; 
                ledState = newState;
                digitalWrite(LED_BUILTIN, ledState);
                
                // Phản hồi xác nhận tới web client
                String response = "{\"status\":\"ok\", \"action\":\"led_control\", \"new_state\":" + String(ledState) + "}";
                webSocket.sendTXT(num, response);
                Serial.println(response);
            }
            break;
        // ... các loại type khác
    }
}

void setupWebSocket() {
    // webSocket.begin(); 
    // webSocket.onEvent(webSocketEvent);
}

void loopWebSocket() {
    // webSocket.loop(); // Lắng nghe WS messages
}

// Hàm đẩy dữ liệu realtime
void sendRealtimeData() {
    // 1. Đọc cảm biến thực tế
    // readTempAndHumi(&temp, &humi); 

    // 2. Gửi dữ liệu qua WS tới TẤT CẢ clients (broadcast)
    String json = "{\"temp\":" + String(temp) + ", \"humi\":" + String(humi) + ", \"led\":" + String(ledState) + "}";
    webSocket.broadcastTXT(json);
} 
// (Cần gọi sendRealtimeData() trong loop() với tần suất cao, ví dụ 100ms)
