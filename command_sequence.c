#include "command_sequence.h"
#include "sas_token_generator.h"

#include "config.h"
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



bool update_command_sequence(iot_config_t *config) 
{
    if (config == NULL) {
        printf("Error: config is NULL\n");
        return false;
    }

    int result = 0;

    result = snprintf(at_command_sequence[0].command, sizeof(at_command_sequence[0].command), "AT+GMR\r\n");
    if (result < 0 || result >= (int)sizeof(at_command_sequence[0].command)) {
        printf("Error: command 0 truncated or failed\n");
        return false;
    }

    result = snprintf(at_command_sequence[1].command, sizeof(at_command_sequence[1].command), "AT+CWMODE=1\r\n");
    if (result < 0 || result >= (int)sizeof(at_command_sequence[1].command)) {
        printf("Error: command 1 truncated or failed\n");
        return false;
    }

    result = snprintf(at_command_sequence[2].command, sizeof(at_command_sequence[2].command), "AT+CWJAP=\"%s\",\"%s\"\r\n", config->wifi_ssid, config->wifi_password);
    if (result < 0 || result >= (int)sizeof(at_command_sequence[2].command)) {
        printf("Error: command 2 truncated or failed\n");
        return false;
    }

    //generate_sas_token(config->password, config->username, 3600, config->sas, sizeof(config->sas));

    result = snprintf(at_command_sequence[3].command, sizeof(at_command_sequence[3].command), "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", config->device_id, config->username, config->sas);
    if (result < 0 || result >= (int)sizeof(at_command_sequence[3].command)) {
        printf("Error: command 3 truncated or failed\n");
        return false;
    }

    result = snprintf(at_command_sequence[4].command, sizeof(at_command_sequence[4].command), "AT+MQTTCONN=0,\"%s\",8883,1\r\n", config->azure_iot_hub_hostname);
    if (result < 0 || result >= (int)sizeof(at_command_sequence[4].command)) {
        printf("Error: command 4 truncated or failed\n");
        return false;
    }

    result = snprintf(at_command_sequence[5].command, sizeof(at_command_sequence[5].command), "AT+MQTTPUB=0,\"devices/%s/messages/events/\",\"{\\\"temperature\\\":25}\",1,0\r\n", config->device_id);
    if (result < 0 || result >= (int)sizeof(at_command_sequence[5].command)) {
        printf("Error: command 5 truncated or failed\n");
        return false;
    }

    return true;
}


