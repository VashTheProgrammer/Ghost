#include "load_config.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool load_config_from_defines(iot_config_t *config) {
    snprintf(config->wifi_ssid, sizeof(config->wifi_ssid), "%s", WIFI_SSID);
    snprintf(config->wifi_password, sizeof(config->wifi_password), "%s", WIFI_PASSWORD);
    snprintf(config->device_id, sizeof(config->device_id), "%s", DEVICE_ID);
    snprintf(config->username, sizeof(config->username), "%s", USERNAME);
    snprintf(config->password, sizeof(config->password), "%s", PASSWORD);
    snprintf(config->azure_iot_hub_hostname, sizeof(config->azure_iot_hub_hostname), "%s", AZURE_IOT_HUB_HOSTNAME);
    return true;
}
