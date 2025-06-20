#include <esp_now.h>
#include <WiFi.h>

#define TRIG_PIN 5
#define ECHO_PIN 18
#define RED_LED 26
#define GREEN_LED 27
#define BLUE_LED 14

HardwareSerial GSM(2); // RX = 16, TX = 17 (يمكنك تعديل الأرجل حسب توصيلك)

// Define the structure for receiving data
typedef struct struct_message {
    float distanceCm;  // Received distance in cm
} struct_message;

struct_message receivedData;
float localDistance = -1;
bool dataReceived = false;

// Callback when ESP-NOW data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    dataReceived = true;

    Serial.print("Received Distance: ");
    if (receivedData.distanceCm == -1) {
        Serial.println("⚠ No valid reading! SR04 Sensor out of range.");
        digitalWrite(RED_LED, HIGH);
        digitalWrite(BLUE_LED, LOW);
    } else {
        Serial.print(receivedData.distanceCm);
        Serial.println(" cm");
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(RED_LED, LOW);
    }
}

// Send SMS via GSM module
void sendGSMMessage(float avg, float local, float remote) {
    String message = String(avg, 2) + "," + String(local, 2) + "," + String(remote, 2);
    
    GSM.println("AT+CMGF=1"); // Text mode
    delay(1000);
    GSM.println("AT+CMGS=\"+201021758066\""); // ⚠ Replace with your phone number
    delay(1000);
    GSM.print(message);
    GSM.write(26); // Ctrl+Z to send
    delay(10000);
}

void setup() {
    Serial.begin(115200);
    GSM.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (Adjust based on wiring)

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);

    WiFi.mode(WIFI_STA);
    Serial.println("✅ ESP32 Receiver is ready!");

    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ ESP-NOW init failed!");
        digitalWrite(RED_LED, HIGH);
        return;
    }

    esp_now_register_recv_cb(onReceive);

    Serial.println("🚀 HC-SR04 Sensor Test Start");
    GSM.println("AT"); // Initialize GSM
    delay(1000);
}

void loop() {
    // Local ultrasonic reading
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    
    if (duration == 0) {
        Serial.println("⚠ No valid reading! HC Sensor out of range.");
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        localDistance = -1;
    } else {
        localDistance = (duration * 0.0343) / 2;
        Serial.print("HC Distance : ");
        Serial.print(localDistance);
        Serial.println(" cm");

        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, LOW);
    }

    if (dataReceived && localDistance > 0 && receivedData.distanceCm > 0) {
        float avg = (localDistance + receivedData.distanceCm) / 2.0;
        sendGSMMessage(avg, localDistance, receivedData.distanceCm);
        dataReceived = false;
    }

    delay(3000);
}