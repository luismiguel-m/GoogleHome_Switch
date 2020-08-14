
//// CHANGE FOR YOUR WIFI SSID AND PASSWORD////
#define WIFI_SSID "NETGEAR"
#define WIFI_PASS "RedHogar1"
///////////////////////////////////////////////

//// CHANGE FOR YOUR IO_USERNAME AND IO_KEY////
#define MQTT_NAME "luismiguel"
#define MQTT_PASS "aio_hgxm455x6RszCOUH9y6KIhk5RFUZ"
///////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define FEED_NAME_ON_OFF "rele_on_off"
#define FEED_STATE "estado_rele"
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
int RELE = D1;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe rele_on_off = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME"/feeds/Pergola");

void setup()
{
  Serial.begin(9600);
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH);

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }
  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&rele_on_off);
}


//probando git

void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &rele_on_off)
    {
      //Print the new value to the serial monitor
      Serial.print("Rele_ON_OFF: ");
      Serial.println((char*) rele_on_off.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) rele_on_off.lastread, "ON"))
      {
        //active low logic
        digitalWrite(RELE, HIGH);
      }
      else if (!strcmp((char*) rele_on_off.lastread, "OFF"))
      {
        digitalWrite(RELE, LOW);
      }
    }
  }
}

void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
