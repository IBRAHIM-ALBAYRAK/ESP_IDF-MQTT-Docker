# ESP_IDF-MQTT-Docker

A project that enables MQTT communication with ESP32-S3, connecting to a cloud-based or Docker container-based MQTT Broker.

Developed using the ESP-IDF framework.
Supports Mosquitto or EMQX Broker running in Docker.
Compatible with HiveMQ and other cloud-based MQTT Brokers.
Allows sending MQTT messages to ESP32-S3 for LED control.

## Supported ESP32 Devices

| ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ------ | ------- | ------- | ------- | ------- | -------- | ------- | ------- | ------- | ------- |
| Yes     | No      | Yes      | No      | Yes      | No       | No      | No      | Yes      | Yes      |

(Only devices with Wi-Fi capability are checked.)

## Project Overview

This project enables ESP32-S3 to act as an MQTT client, communicating with an MQTT Broker running on Docker or in the cloud.

ESP32-S3 connects to an MQTT Broker over Wi-Fi and exchanges messages over a specified topic. For example:

- ESP32-S3 subscribes to the "led/control" topic.
- If the MQTT message is "on", the LED turns on.
- If the MQTT message is "off", the LED turns off.
- Can be used with a Docker-hosted Mosquitto or EMQX broker.

## Requirements

- ESP32-S3 Development Board
- ESP-IDF 5.0+ installed on a system (Ubuntu recommended)
- Mosquitto or EMQX (Docker or cloud-based)
- Wi-Fi connection

## Installation & Usage Steps

### Clone the Project
```bash
git clone https://github.com/IBRAHIM-ALBAYRAK/ESP_IDF-MQTT-Docker.git
cd ESP_IDF-MQTT-Docker
```

### Configure Wi-Fi and MQTT Settings

Modify the `main.c` file with your Wi-Fi and MQTT Broker details:

```c
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASS "Your_WiFi_Password"
#define MQTT_BROKER_URI "mqtt://broker.hivemq.com" // Or your Docker-hosted broker
#define LED_GPIO GPIO_NUM_19
```

### Install Dependencies for ESP32
```bash
idf.py fullclean
idf.py set-target esp32s3
idf.py menuconfig
```
Inside ESP-IDF menuconfig:
- Go to "Component Config" → "ESP32-specific" → Enable SPIFFS.

### Build & Flash the Firmware

To upload the firmware to ESP32-S3:
```bash
idf.py build flash monitor
```
Press `Ctrl + ]` to exit the monitor.

## Running an MQTT Broker with Docker

If you prefer to use a self-hosted MQTT broker instead of HiveMQ, you can run Mosquitto on Docker:

Start the Mosquitto MQTT Broker in Docker:
```bash
docker run -d --name mosquitto -p 1883:1883 -p 9001:9001 eclipse-mosquitto
```

Update `MQTT_BROKER_URI` in the source code:
```c
#define MQTT_BROKER_URI "mqtt://192.168.1.100"  // IP of the device running Docker
```

## Sending MQTT Commands to Test ESP32

### Control the LED via MQTT

Use the following commands in the terminal to test your ESP32:

Turn the LED ON:
```bash
mosquitto_pub -h broker.hivemq.com -t "led/control" -m "on"
```

Turn the LED OFF:
```bash
mosquitto_pub -h broker.hivemq.com -t "led/control" -m "off"
```

Listen for messages sent to ESP32:
```bash
mosquitto_sub -h broker.hivemq.com -t "led/control"
```

If using your own MQTT Broker in Docker, replace `broker.hivemq.com` with your own IP address.

## Expected Serial Monitor Output

If everything is working correctly, ESP32 will print the following output in the serial monitor:

```
I (2008) esp_netif_handlers: sta ip: 192.168.1.34, mask: 255.255.255.0, gw: 192.168.1.1
Wi-Fi Connection Successful!
I (5569) main_task: Returned from app_main()
I (8000) MQTT_APP: MQTT connected!
I (8012) MQTT_APP: Incoming message: "on"
LED Turned ON!
```

## Erasing ESP32 Flash Memory (If Needed)

If you need to erase ESP32's memory:
```bash
idf.py erase-flash
idf.py flash monitor
```

## Troubleshooting

| Issue | Solution |
|-------------------------------------|-----------|
| `MQTT connect failed` | Ensure MQTT Broker IP is correct! Is Docker running? |
| `Wi-Fi Disconnected!` | Check SSID & Password. Is the connection stable? |
| `mosquitto_pub: Connection refused` | Ensure MQTT Broker is running! Check with `docker ps`. |

## About the Project

This project demonstrates ESP32-S3 communication with MQTT, allowing IoT devices to interact with cloud-based or Docker-based solutions.

Great starting point for developers looking to integrate ESP32 with MQTT!

