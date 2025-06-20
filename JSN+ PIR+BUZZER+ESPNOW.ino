#include <esp_now.h>
#include <WiFi.h>

// === Pin Definitions ===
#define TRIG_PIN     18  // JSN TRIG
#define ECHO_PIN     5   // JSN ECHO
#define PIR_PIN      22  // PIR sensor
#define BUZZER_PIN   26  // Buzzer

#define SOUND_SPEED  0.0343  // cm/us

// === ESP-NOW Struct ===
typedef struct struct_message {
    float distanceCm;  // Measured distance in cm
} struct_message;

struct_message sensorData;

// === Receiver MAC Address ===
uint8_t receiverMAC[] = {0x08, 0xA6, 0xF7, 0xBC, 0xE9, 0xB0};

// === Callback for Send Status ===
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Data sent: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void setup() {
  Serial.begin(115200);

  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // === WiFi & ESP-NOW Setup ===
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW Init Failed!");
    return;
  }

  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Failed to add peer");
    return;
  }

  Serial.println("🚀 System Ready: JSN + PIR + ESP-NOW");
}

void loop() {
  // === 1. Read Distance from JSN ===
  float distance = readDepth();

  if (distance == -1.0) {
    Serial.println("⚠ No echo received! Out of range.");
    sensorData.distanceCm = -1;
  } else {
    Serial.print("📏 Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    sensorData.distanceCm = distance;
  }

  // === 2. Read PIR & Control Buzzer ===
  bool motion = digitalRead(PIR_PIN);
  if (motion) {
    Serial.println("⚠ Motion Detected → Buzzer ON");
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    Serial.println("✅ No Motion → Buzzer OFF");
    digitalWrite(BUZZER_PIN, LOW);
  }

  // === 3. Send data using ESP-NOW ===
  esp_now_send(receiverMAC, (uint8_t *) &sensorData, sizeof(sensorData));

  Serial.println("--------------------------------");
  delay(1000);
}

// === JSN-SR04T Reading Function ===
float readDepth() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout: 30ms

  if (duration == 0) return -1.0;

  float distance = (duration * SOUND_SPEED) / 2;
  return distance;
}
