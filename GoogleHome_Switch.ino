
//// CHANGE FOR YOUR WIFI SSID AND PASSWORD////
#define WIFI_SSID "BSSID"
#define WIFI_PASS "Pass-WIFI"
///////////////////////////////////////////////

//// CHANGE FOR YOUR IO_USERNAME AND IO_KEY////
#define MQTT_NAME "user"
#define MQTT_PASS "pass"
///////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define FEED_NAME_ON_OFF "rele_on_off"
#define FEED_STATE "estado_rele"
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
const byte switchHome = D5;
int Relay = D3; //Pin to avoid ESP.reset() flickers
int lightState = LOW;
int actualSwitchState;
int lastSwitchState;


/////////////////////////////////////////////////

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 150;    // the debounce time; increase if the output flickers



WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe rele_on_off = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME"/feeds/Pergola");

void setup()
{  
  pinMode(Relay, OUTPUT);

  //Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(50);
  }

  //Subscribe to the onoff topic
  mqtt.subscribe(&rele_on_off);

  pinMode(switchHome, INPUT);
  attachInterrupt(digitalPinToInterrupt(switchHome), switchChangeDetector, CHANGE);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !lightState);

  lastSwitchState = digitalRead(switchHome);
}


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

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) rele_on_off.lastread, "ON"))
      {
        //active low logic
        digitalWrite(Relay, HIGH);
        lightState = HIGH;
        digitalWrite(LED_BUILTIN, !lightState);
      }
      else if (!strcmp((char*) rele_on_off.lastread, "OFF"))
      {
        //active low logic
        digitalWrite(Relay, LOW);
        lightState = LOW;
        digitalWrite(LED_BUILTIN, !lightState);
      }
    }
  }
}

void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
}

ICACHE_RAM_ATTR void switchChangeDetector() {

  if ((millis() - lastDebounceTime) > debounceDelay) {

    actualSwitchState = digitalRead(switchHome);
    
    if (lastSwitchState != actualSwitchState) {
      
      lightState = !(lightState);
      digitalWrite(Relay, lightState);
      
      // set the internal LED:
      digitalWrite(LED_BUILTIN, !lightState);
      
      lastSwitchState = digitalRead(switchHome);
    }

  }

  lastDebounceTime = millis();
}
