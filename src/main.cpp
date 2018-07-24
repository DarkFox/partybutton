#include <Arduino.h>

#include "Secrets.h"

#include <string>

#include <LEDFader.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>   // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>        // WiFi Configuration Magic
#include <PubSubClient.h>       // MQTT client
#include <WiFiUdp.h>

#define DEVELOPMENT

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
ESP8266WebServer httpServer(80);

// buffer used to send/receive data with MQTT
const uint16_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE];

#define PWMRANGE 255

ADC_MODE(ADC_VCC); // to use getVcc

#define LED_RED_PIN    13
#define LED_GREEN_PIN  14
//#define LED_BUTTON_PIN 12
LEDFader led = LEDFader(12);

#define BUTTON_PIN     5

// store the state of the button light (brightness, ...)
boolean m_button_state = false;

boolean buttonState;             // the current reading from the input pin
boolean lastButtonState = false; // the previous reading from the input pin

u_int64_t lastDebounceTime = 0;  // the last time the output pin was toggled
uint16_t debounceDelay = 50;    // the debounce time; increase if the output flickers

u_int64_t last_mqtt_publish = 0;

// function called to publish the state of the button (on/off)
void publishButtonState() {
	if (m_button_state) {
		client.publish(MQTT_BUTTON_STATE_TOPIC, CMD_ON, true);
		Serial.println("Button ON");
	}
	else {
		client.publish(MQTT_BUTTON_STATE_TOPIC, CMD_OFF, true);
		Serial.println("Button OFF");
	}
}

void publishBatteryState() {
	Serial.print("ESP Vcc: ");
	Serial.println(ESP.getVcc()/1000);
	snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%u", ESP.getVcc());
	client.publish(MQTT_BATTERY_STATE_TOPIC, m_msg_buffer, true);
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
	// concat the payload into a string
	String payload;
	for (uint16_t i = 0; i < p_length; i++) {
		payload.concat((char)p_payload[i]);
	}

	// handle message topic
	if (String(MQTT_BUTTON_COMMAND_TOPIC).equals(p_topic)) {
		// test if the payload is equal to "ON" or "OFF"
		if (payload.equals(String(CMD_ON))) {
			m_button_state = true;
			publishButtonState();
		}
		else if (payload.equals(String(CMD_OFF))) {
			m_button_state = false;
			publishButtonState();
		}
	}
}

void setup()
{
	pinMode(LED_RED_PIN, OUTPUT);
	pinMode(LED_GREEN_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);

	digitalWrite(LED_RED_PIN, 1);
	digitalWrite(LED_GREEN_PIN, 0);

	sprintf(chip_id, "%08X", ESP.getChipId());
	sprintf(myhostname, "esp%08X", ESP.getChipId());

	// Setup console
	Serial.begin(115200);
	delay(10);
	Serial.println();
	Serial.println();

	// reset if necessary
	//wifiManager.resetSettings();

	wifiManager.setTimeout(3600);
	WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
	wifiManager.addParameter(&custom_mqtt_server);
	wifiManager.setCustomHeadElement(chip_id);
	wifiManager.autoConnect();

	mqtt_server = custom_mqtt_server.getValue();

	Serial.println("");

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.println("");

	// init the MQTT connection
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

	// replace chip ID in channel names
	memcpy(MQTT_BUTTON_STATE_TOPIC, chip_id, 8);
	memcpy(MQTT_BATTERY_STATE_TOPIC, chip_id, 8);
	memcpy(MQTT_BUTTON_COMMAND_TOPIC, chip_id, 8);
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		digitalWrite(LED_GREEN_PIN, 0);
		led.fade(0, 0);
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect(chip_id, mqtt_user, mqtt_password)) {
			Serial.println("connected");

			client.publish(MQTT_UP, chip_id);

			// Once connected, publish an announcement...
			publishBatteryState();

			// ... and resubscribe
			client.subscribe(MQTT_BUTTON_COMMAND_TOPIC);

			digitalWrite(LED_GREEN_PIN, 1);
			led.fade(255, 0);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        m_button_state = !m_button_state;
				publishButtonState();
      }
    }
  }

  lastButtonState = reading;

	led.update();

	if (led.is_fading() == false) {

    // Fade from 255 - 0
    if (led.get_value() == 255 && m_button_state == true) {
      led.fade(0, 1500);
    }
    // Fade from 0 - 255
    else {
      led.fade(255, 1500);
    }
  }

	if ((millis() - last_mqtt_publish) > 60000) {
		publishButtonState();
		publishBatteryState();
		last_mqtt_publish = millis();
	}
}
