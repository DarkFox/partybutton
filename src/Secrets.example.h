#pragma once

const char* mqtt_server = "x.x.x.x";
const int mqtt_port = 1883;
const char* mqtt_user = "USERNAME";
const char* mqtt_password = "PASSWORD";

// Light
// the payload that represents enabled/disabled state, by default
const char* CMD_ON = "ON";
const char* CMD_OFF = "OFF";

// MQTT topics
// state, command
const char* MQTT_UP = "active";
char* MQTT_BUTTON_STATE_TOPIC = (char*)"XXXXXXXX/party/button/status";
char* MQTT_BATTERY_STATE_TOPIC = (char*)"XXXXXXXX/party/battery/status";
char* MQTT_BUTTON_COMMAND_TOPIC = (char*)"XXXXXXXX/party/button/switch";

char* chip_id = (char*)"00000000";
char* myhostname = (char*)"esp00000000";
