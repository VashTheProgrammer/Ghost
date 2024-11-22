#ifndef IOT_CONFIG_H
#define IOT_CONFIG_H

#define WIFI_SSID_MAX_LEN 128
#define WIFI_PASSWORD_MAX_LEN 128
#define DEVICE_ID_MAX_LEN 32
#define USERNAME_MAX_LEN 512
#define PASSWORD_MAX_LEN 512
#define AZURE_IOT_HUB_HOSTNAME_MAX_LEN 256

typedef struct {
    char wifi_ssid[WIFI_SSID_MAX_LEN];
    char wifi_password[WIFI_PASSWORD_MAX_LEN];
    char device_id[DEVICE_ID_MAX_LEN];
    char username[USERNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
    char azure_iot_hub_hostname[AZURE_IOT_HUB_HOSTNAME_MAX_LEN];
} iot_config_t;

#endif // IOT_CONFIG_H
