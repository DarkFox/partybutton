#pragma once

const char* mqtt_server = "x.x.x.x";
const int mqtt_port = 1883;
const char* mqtt_user = "USERNAME";
const char* mqtt_password = "PASSWORD";

// Light
// the payload that represents enabled/disabled state, by default
const char* CMD_ON = "ON";
const char* CMD_OFF = "OFF";

const char* AVAIL_ONLINE = "online";
const char* AVAIL_OFFLINE = "offline";


// MQTT topics
// state, command
const char* MQTT_UP = "active";
char* MQTT_BUTTON_STATE_TOPIC   = (char*)"XXXXXXXX/partybutton/status";
char* MQTT_BUTTON_COMMAND_TOPIC = (char*)"XXXXXXXX/partybutton/switch";
char* MQTT_BATTERY_STATE_TOPIC  = (char*)"XXXXXXXX/partybutton/battery";
char* MQTT_AVAILABILITY_TOPIC   = (char*)"XXXXXXXX/partybutton/available";

char* chip_id = (char*)"00000000";
char* myhostname = (char*)"esp00000000";
