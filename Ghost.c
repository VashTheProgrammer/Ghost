#include <stdio.h>
#include "pico/stdlib.h"
#include "command_sequence.h"
#include "state_machine.h"
#include "config.h"

#include "az_iot.h"
#include "az_iot_hub_client.h"

#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define LED_PIN 25

#define BAUD_RATE 115200
#define COMMAND_INTERVAL_MS 10000  // Intervallo di tempo tra i comandi AT

int main() {

    // Inizializza la configurazione della UART
    stdio_init_all();
    
    // Inizializza UART0 (connessione al PC)
    uart_init(uart0, BAUD_RATE);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
    
    // Inizializza UART1 (connessione all'ESP32)
    uart_init(uart1, BAUD_RATE);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

    printf("\n\n");

    // Inizializza il pin del LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Variabile per il controllo del tempo per il blink del LED
    absolute_time_t led_time = make_timeout_time_ms(500);
    bool led_on = false;
    absolute_time_t command_time = make_timeout_time_ms(2000);  // Prima attesa di 2 secondi
    int at_command_counter = 0;

    // Aggiorna i comandi con i parametri configurati
    update_command_sequence(&iot_config);

    // UN PO DI MERDA PER TE
    // -----------------------------------------------------------------------------

    char *iot_hub_hostname = "your-iothub.azure-devices.net";
    char *device_id = "your-device-id";
    char *device_key = "your-device-key";

    az_iot_hub_client hub_client;
    az_iot_hub_client_options options = az_iot_hub_client_options_default();
    az_result result = az_iot_hub_client_init(
      &hub_client,
      az_span_create_from_str(iot_hub_hostname),
      az_span_create_from_str(device_id),
      &options);

    if (result != AZ_OK) {
        printf("Errore nella configurazione del client IoT Hub\n");
    } 
    else {
        printf("Client IoT Hub configurato correttamente\n");
    }

    // -----------------------------------------------------------


    // Stato della macchina a stati
    at_command_state_t state = STATE_WAIT_BEFORE_START;

    // Loop principale per fare da bridge tra le due UART, gestire la macchina a stati, e blink del LED
    while (true) {
        // Controlla se è il momento di cambiare lo stato del LED
        if (absolute_time_diff_us(get_absolute_time(), led_time) <= 0) {
            led_on = !led_on;
            gpio_put(LED_PIN, led_on);
            led_time = make_timeout_time_ms(500);
        }

        // Gestisci la macchina a stati
        state_machine(&state, &command_time, &led_time, &at_command_counter);

        // Se c'è un dato disponibile sulla UART0 (PC), invialo alla UART1 (ESP32)
        if (uart_is_readable(uart0)) {
            uint8_t data = uart_getc(uart0);
            uart_putc(uart1, data);
        }

        // Se c'è un dato disponibile sulla UART1 (ESP32), invialo alla UART0 (PC)
        if (uart_is_readable(uart1)) {
            uint8_t data = uart_getc(uart1);
            uart_putc(uart0, data);
        }
    }
}
