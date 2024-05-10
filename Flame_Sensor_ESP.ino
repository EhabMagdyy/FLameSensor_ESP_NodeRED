/*****************************************************************************************************************************
**********************************    Author  : Ehab Magdy Abdullah                      *************************************
**********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *************************************
**********************************    Youtube : https://www.youtube.com/@EhabMagdyy      *************************************
******************************************************************************************************************************/
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif 
#include <PubSubClient.h>

/* Analog pin connected to the AO of Flame sensor */
#define FLAME_AN_PIN       35                   /* Note: Do not select any ADC2 pin, beacuse it doesn't work when using WIFI */

const char* ssid = "2001";                        /* Your Wifi SSID */
const char* password = "19821968";                /* Your Wifi Password */
const char* mqtt_server = "test.mosquitto.org";   /* Mosquitto Server URL */

WiFiClient espClient;
PubSubClient client(espClient);

uint16_t analog_flame_value = 0;
uint16_t flame_percentage = 0;

void setup_wifi()
{ 
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) 
    { 
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() 
{ 
    while(!client.connected()) 
    {
        Serial.println("Attempting MQTT connection...");

        if(client.connect("ESPClient")) 
        {
            Serial.println("Connected");
            client.subscribe("Ehab/Flame");
        } 
        else 
        {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi(); 
    client.setServer(mqtt_server, 1883);
}

void loop()
{
    if(!client.connected()) { reconnect(); }

    /* Read value of Flame Sensor and map it */
    analog_flame_value = analogRead(FLAME_AN_PIN);
    flame_percentage = map(analog_flame_value, 0, 4095, 100, 0);

    char f[4] = {0};

    f[0] = flame_percentage / 100 + '0';
    f[1] += (flame_percentage / 10) % 10 + '0';
    f[2] += flame_percentage % 10 + '0';

    /* Sending Data to Node-Red */
    client.publish("Ehab/Flame", f, false);

    Serial.print("Flame: ");
    Serial.print(flame_percentage);
    Serial.println("%");

    delay(250);
}