#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Định nghĩa thông tin kết nối
char auth[] = "BLYNK_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Định nghĩa các Virtual Pin
#define VPIN_TEMP V5
#define VPIN_HUMI V6
#define VPIN_LED V7
#define VPIN_ALERT V10

// Biến trạng thái
float temp = 0.0;
float humi = 0.0;
int ledState = LOW; // Trạng thái LED

// Hàm được gọi khi Blynk App gửi lệnh đến VPIN_LED
BLYNK_WRITE(VPIN_LED) {
    int pinValue = param.asInt();
    if (pinValue == 1) {
        // Bật LED vật lý
        digitalWrite(LED_BUILTIN, HIGH);
        ledState = HIGH;
    } else {
        // Tắt LED vật lý
        digitalWrite(LED_BUILTIN, LOW);
        ledState = LOW;
    }
    // Phản hồi trạng thái LED ngược lại (quan trọng)
    Blynk.virtualWrite(VPIN_LED, ledState);
}

void setup() {
    Serial.begin(115200);
    // Khởi tạo LED vật lý
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState);

    // Bắt đầu kết nối Wi-Fi và Blynk
    Blynk.begin(auth, ssid, pass);
    
    // Đặt Timer để gửi dữ liệu định kỳ
    // timer.setInterval(2000L, sendSensorData); 
}

void sendSensorData() {
    // 1. Đọc cảm biến thực tế (thay thế bằng hàm đọc thực tế)
    // readTempAndHumi(&temp, &humi); 

    // 2. Gửi dữ liệu lên Blynk
    Blynk.virtualWrite(VPIN_TEMP, temp);
    Blynk.virtualWrite(VPIN_HUMI, humi);

    // 3. Cảnh báo khi nhiệt độ vượt ngưỡng
    if (temp > 35.0) {
        Blynk.logEvent("temp_high_warning", "Nhiệt độ đã vượt ngưỡng an toàn!");
    }
}

void loop() {
    // Chạy các tác vụ của Blynk (kiểm tra kết nối, nhận lệnh)
    Blynk.run(); 
    // timer.run(); 
}
