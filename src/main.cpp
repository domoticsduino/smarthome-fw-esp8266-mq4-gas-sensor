/* 1.0.0 VERSION */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "../include/config.h"

#include <ddcommon.h>
#include <ddwifi.h>
#include <ddmqtt.h>
#include <ddmq4.h>

int currentSample;

//Wifi
DDWifi wifi(ssid, password, wifihostname, LEDSTATUSPIN);

//MQTT
DDMqtt clientMqtt(DEVICE, MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PWD, TOPIC_S, MQTT_QOS, LEDSTATUSPIN);

//MQ-4 Gas sensor
DDMQ4 mq4Sensor(MQ4PIN);
DDMQ4Val sampleGasValues;
int countSampleGas;

//JSON
DynamicJsonBuffer jsonBuffer;
JsonObject &root = jsonBuffer.createObject();
JsonObject &configRoot = root.createNestedObject("config");
JsonObject &gas = root.createNestedObject("gas");
JsonObject &jsonInfo = root.createNestedObject("info");

// WEB SERVER - OTA
AsyncWebServer server(80);

void printDebugGas(DDMQ4Val gasValue)
{
	writeToSerial("GAS Success = ", false);
	writeToSerial(gasValue.success ? "True" : "False", true);
	if (!gasValue.success)
		writeToSerial(gasValue.errorMsg, true);
	else
	{
		writeToSerial("analog = ", false);
		writeToSerial(gasValue.sensorValue, true);
		writeToSerial("ppm = ", false);
		writeToSerial(gasValue.ppm, true);
		writeToSerial("percentage = ", false);
		writeToSerial(gasValue.percentage, false);
		writeToSerial(" %", true);
		writeToSerial("real value = ", false);
		writeToSerial(gasValue.realValue, true);
	}
}

String generateJsonMessage()
{
	DDMQ4Val gasValueTot;
	if (countSampleGas > 0)
	{
		gasValueTot.sensorValue = sampleGasValues.sensorValue / countSampleGas;
		gasValueTot.ppm = sampleGasValues.ppm / countSampleGas;
		gasValueTot.percentage = sampleGasValues.percentage / countSampleGas;
		gasValueTot.realValue = sampleGasValues.realValue / countSampleGas;
		gas["error"] = "";
	}
	else
		gas["error"] = "No samples";
	gas["sensorValue"] = gasValueTot.sensorValue;
	gas["ppm"] = gasValueTot.ppm;
	gas["percentage"] = gasValueTot.percentage;
	gas["realValue"] = gasValueTot.realValue;
	String json;
	root.printTo(json);
	return json;
}

void createJsonConfig()
{
	configRoot["readInterval"] = READ_INTERVAL;
	configRoot["numSamples"] = NUM_SAMPLES;
	configRoot["warmupTime"] = WARMUP_TIME;
}

void setup()
{
	createJsonConfig();

	pinMode(LEDSTATUSPIN, OUTPUT);
	pinMode(MQ4PIN, INPUT);

	digitalWrite(LEDSTATUSPIN, LOW);

	if (SERIAL_ENABLED)
		Serial.begin(SERIAL_BAUDRATE);

	writeToSerial("ESP8266MCU11 Booting...", true);
	writeToSerial("FW Version: ", false);
	writeToSerial(AUTO_VERSION, true);

	// WIFI
	wifi.connect();

	//MQTT
	clientMqtt.reconnectMQTT();

	//WEB SERVER
	jsonInfo["name"] = USER_SETTINGS_WIFI_HOSTNAME;
	jsonInfo["version"] = AUTO_VERSION;
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "application/json", generateJsonMessage());
  });

	AsyncElegantOTA.begin(&server);
  server.begin();
	writeToSerial("Http server started", true);

	// MQ4
	countSampleGas = 0;
	currentSample = 0;

	// WARM UP
	writeToSerial("Warming up sensors", true);
	int i;
	for (i = 0; i < configRoot["warmupTime"]; i++)
	{
		digitalWrite(LEDSTATUSPIN, LOW);
		delay(500);
		digitalWrite(LEDSTATUSPIN, HIGH);
		delay(500);
		writeToSerial(".", false);
	}

	writeToSerial("", true);
	writeToSerial("Warmup finished", true);
}

void loop()
{
	AsyncElegantOTA.loop();

	delay(configRoot["readInterval"]);
	clientMqtt.loop();

	// Get GAS Value
	DDMQ4Val gasValue = mq4Sensor.getValue();
	printDebugGas(gasValue);

	if (gasValue.success)
	{
		sampleGasValues.sensorValue += gasValue.sensorValue;
		sampleGasValues.ppm += gasValue.ppm;
		sampleGasValues.percentage += gasValue.percentage;
		sampleGasValues.realValue += gasValue.realValue;
		countSampleGas++;
	}

	writeToSerial("currentSample ", false);
	writeToSerial(currentSample, true);

	if (++currentSample >= configRoot["numSamples"])
	{
		clientMqtt.sendMessage(TOPIC_P, generateJsonMessage());
		currentSample = 0;
		countSampleGas = 0;
		sampleGasValues.sensorValue = 0.0;
		sampleGasValues.ppm = 0.0;
		sampleGasValues.percentage = 0.0;
		sampleGasValues.realValue = 0.0;
	}
}
