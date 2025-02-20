
/*
    ----------------------------------------------------------------------------
    |                      MQTT DOCKER-BASED PROJECT                          |
    ----------------------------------------------------------------------------
    | This project is designed to work with MQTT in a Docker environment.      |
    | The ESP32-S3 device acts as an MQTT client, while the broker runs        |
    | either on a cloud-based MQTT service (HiveMQ) or inside a Docker         |
    | container running Mosquitto or EMQX.                                      |
    |                                                                          |
    | Key Features:                                                            |
    | - ESP32-S3 connects to Wi-Fi and communicates over MQTT                  |
    | - Works with cloud-based or self-hosted MQTT brokers                      |
    | - Can be integrated with a Dockerized backend system                      |
    |                                                                          |
    | MQTT Broker Options:                                                      |
    | - Public MQTT Broker: HiveMQ (broker.hivemq.com)                          |
    | - Dockerized Broker: Eclipse Mosquitto or EMQX                            |
    |                                                                          |
    | Run MQTT Broker in Docker (If Using Mosquitto):                           |
    | $ docker run -d --name mosquitto -p 1883:1883 -p 9001:9001 eclipse-mosquitto |
    |                                                                          |
    | Publish Test Messages from Linux:                                         |
    | $ mosquitto_pub -h broker.hivemq.com -t "led/control" -m "on"             |
    | $ mosquitto_pub -h broker.hivemq.com -t "led/control" -m "off"            |
    ----------------------------------------------------------------------------
*/


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"


#define WIFI_SSID "WIFI_SSID"  // write the your WI-FI SSD
#define WIFI_PASS "WIFI_SSID"  // write the your WI-FI password


#define MQTT_BROKER_URI "mqtt://broker.hivemq.com"
#define LED_GPIO GPIO_NUM_19  

static const char *TAG = "MQTT_Application";


/* Functions Definitions */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_init(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void) ;





void app_main(void) 
{
    nvs_flash_init();  
    wifi_init();       // Start the WI-FI

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    vTaskDelay(pdMS_TO_TICKS(2500));  // wait to connect the WI-FI
    mqtt_app_start();  // Start the MQTT
}




/* Functions Declarations */


// *** WI-FI Event Handler ***
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        printf("WI-FI Connection Successful!\n");
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        printf("WI-FI Disconnected! Reconnecting...\n");
        esp_wifi_connect();
    }
}

// *** Start WI-FI Connection***
void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

// *** MQTT Event Handler ***
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected!");
            esp_mqtt_client_subscribe(client, "led/control", 0);
            break;
        case MQTT_EVENT_DATA:
            printf("incoming message: %.*s\n", event->data_len, event->data);

            if (strncmp(event->data, "on", event->data_len) == 0) {
                gpio_set_level(LED_GPIO, 1);
                printf("LED opened\n");
            } else if (strncmp(event->data, "off", event->data_len) == 0) {
                gpio_set_level(LED_GPIO, 0);
                printf("LED closed!\n");
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Bağlantısı Kesildi!");
            break;
        default:
            break;
    }
}

// *** Start the MQTT Connection ***
void mqtt_app_start(void) 
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,  
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}


