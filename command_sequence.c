#include "command_sequence.h"
#include <stdio.h>

at_command_t at_command_sequence[] = {
    {STATE_SEND_AT_GMR, "", "Inviando AT+GMR alla UART1", STATE_SEND_AT_CWMODE},
    {STATE_SEND_AT_CWMODE, "", "Inviando AT+CWMODE=1 alla UART1", STATE_SEND_AT_CONNECT_WIFI},
    {STATE_SEND_AT_CONNECT_WIFI, "", "Inviando AT+CWJAP alla UART1", STATE_SEND_AT_MQTT_CONFIG},
    {STATE_SEND_AT_MQTT_CONFIG, "", "Configurazione MQTT per Azure IoT Hub", STATE_SEND_AT_MQTT_CONNECT},
    {STATE_SEND_AT_MQTT_CONNECT, "", "Connessione MQTT a Azure IoT Hub", STATE_SEND_AT_MQTT_PUBLISH},
    {STATE_SEND_AT_MQTT_PUBLISH, "", "Pubblicazione messaggio MQTT su Azure IoT Hub", STATE_COMPLETE},
    {STATE_COMPLETE, "", "Configurazione completata.", STATE_IDLE}
};

void update_command_sequence(iot_config_t *config) {
    snprintf(at_command_sequence[0].command, sizeof(at_command_sequence[0].command), "AT+GMR\r\n");
    snprintf(at_command_sequence[1].command, sizeof(at_command_sequence[1].command), "AT+CWMODE=1\r\n");
    snprintf(at_command_sequence[2].command, sizeof(at_command_sequence[2].command), "AT+CWJAP=\"%s\",\"%s\"\r\n", config->wifi_ssid, config->wifi_password);
    snprintf(at_command_sequence[3].command, sizeof(at_command_sequence[3].command), "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", config->device_id, config->username, config->password);
    snprintf(at_command_sequence[4].command, sizeof(at_command_sequence[4].command), "AT+MQTTCONN=0,\"%s\",8883,1\r\n", config->azure_iot_hub_hostname);
    snprintf(at_command_sequence[5].command, sizeof(at_command_sequence[5].command), "AT+MQTTPUB=0,\"devices/%s/messages/events/\",\"{\\\"temperature\\\":25}\",1,0\r\n", config->device_id);
}
