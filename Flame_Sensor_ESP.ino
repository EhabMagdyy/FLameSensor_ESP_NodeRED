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
#define FLAME_AN_PIN       35               /* Note: Do not select any ADC2 pin, beacuse it doesn't work when using WIFI */

#define ssid "2001"                         /* Your Wifi SSID */
#define password "19821968"                 /* Your Wifi Password */
#define mqtt_server "test.mosquitto.org"    /* Mosquitto Server URL */

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

        if(client.connect("EhabESPClient")) 
        {
            Serial.println("Connected");
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
    if (!client.connected()){ reconnect(); }

    client.loop(); 

    // Read value of Flame Sensor and map it
    analog_flame_value = analogRead(FLAME_AN_PIN);
    flame_percentage = map(analog_flame_value, 0, 4095, 100, 0);

    String payload = String(flame_percentage);

    // Send data to Node-RED
    client.publish("Ehab/Flame", payload.c_str());

    Serial.print("Flame: ");
    Serial.print(flame_percentage);
    Serial.println("%");

    delay(1000);
}
