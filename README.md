
# 🌊 ESP32 Water Level Monitoring System with ESP-NOW & GSM

## 📋 Project Overview

This project implements a wireless water level monitoring system using **two ESP32 boards**:

- **Node 1 (Transmitter)**: Uses a **JSN-SR04T ultrasonic sensor** and a **PIR motion sensor** to measure water level and detect motion. It sends the measured data using **ESP-NOW**.
- **Node 2 (Receiver)**: Collects the data via ESP-NOW and uses a **GSM module** to forward it to a cloud server:

**Server URL**: [`https://lakewaterlevel.onrender.com/`](https://lakewaterlevel.onrender.com/)

---

## 📡 Hardware Components

### Transmitter Node
- ESP32
- JSN-SR04T Waterproof Ultrasonic Sensor
- PIR Motion Sensor
- Buzzer
- Jumper wires

### Receiver Node
- ESP32
- HC-SR04 Ultrasonic Sensor
- GSM Module (e.g., SIM800L)
- RGB LEDs (Red, Green, Blue)
- Jumper wires

---

## 🔁 System Workflow

```
[ JSN Sensor + PIR ] --> [ Transmitter ESP32 ] --ESP-NOW--> [ Receiver ESP32 ] --> [ GSM Module ] --> [ Server ]
```

---

## 🧠 Features

### Transmitter ESP32
- Measures water level using JSN-SR04T.
- Detects motion via PIR sensor.
- Activates buzzer on motion.
- Sends distance data to receiver via ESP-NOW.

### Receiver ESP32
- Receives remote distance via ESP-NOW.
- Measures local distance via HC-SR04.
- Averages both distances.
- Sends data via GSM (SMS or HTTP).
- Uses RGB LEDs for feedback:
  - 🔴 Red: No valid reading
  - 🟢 Green: Valid local reading
  - 🔵 Blue: Valid remote reading

---

## 🌐 Server Integration

Currently, the GSM module sends SMS data, but it can also be configured to make HTTP POST requests to:

```
https://lakewaterlevel.onrender.com/
```

**Note:** Make sure your server supports JSON or CSV formatted data.

---

## 🛠 Setup Instructions

### Transmitter (ESP-NOW Sender)
1. Flash `JSN + PIR + Buzzer + ESP-NOW.c.txt` to your ESP32.
2. Wire JSN-SR04T and PIR sensor.
3. Connect buzzer to specified pin.
4. Update `receiverMAC[]` with your receiver ESP32 MAC address.

### Receiver (ESP-NOW + GSM)
1. Flash `System with Gsm.txt` to the second ESP32.
2. Connect HC-SR04, GSM module, and RGB LEDs.
3. Replace the phone number in `sendGSMMessage()` with your own (if using SMS).
4. Alternatively, modify code to POST data to the server.

---

## 📝 Sample Output

### Transmitter Serial Log
```
📏 Distance: 67.50 cm
⚠ Motion Detected → Buzzer ON
Data sent: Success
```

### Receiver Serial Log
```
Received Distance: 67.50 cm
HC Distance : 66.20 cm
GSM Sent: 66.85,66.20,67.50
```

---

## 🚀 Future Improvements
- Replace SMS with HTTP POST to server.
- Add GPS or timestamp to data packets.
- Enable OTA firmware updates.
- Solar + battery powered operation.

---

## 👤 Author

**Ahmed Mohamed**  
Developed using ESP32, ESP-NOW, and GSM for IoT water level monitoring.

---
