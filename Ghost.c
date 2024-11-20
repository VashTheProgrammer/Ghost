#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>

#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define LED_PIN 25

#define BAUD_RATE 115200
#define COMMAND_INTERVAL_MS 10000  // Intervallo di tempo tra i comandi AT

typedef enum {
    STATE_WAIT_BEFORE_START,
    STATE_SEND_AT_RST,
    STATE_WAIT_AFTER_RST,
    STATE_SEND_AT_GMR,
    STATE_SEND_AT_CWMODE,
    STATE_SEND_AT_CONNECT_WIFI,
    STATE_COMPLETE,
    STATE_IDLE
} at_command_state_t;

typedef struct {
    at_command_state_t state;
    const char *command;
    const char *message;
    at_command_state_t next_state;
} at_command_t;

at_command_t at_command_sequence[] = {
    {STATE_SEND_AT_GMR, "AT+GMR\r\n", "Inviando AT+GMR alla UART1", STATE_SEND_AT_CWMODE},
    {STATE_SEND_AT_CWMODE, "AT+CWMODE=1\r\n", "Inviando AT+CWMODE=1 alla UART1", STATE_SEND_AT_CONNECT_WIFI},
    {STATE_SEND_AT_CONNECT_WIFI, "AT+CWJAP=\"Vashphone\",\"pippopluto69aba\"\r\n", "Inviando AT+CWJAP=\"SSID\",\"PASSWORD\" alla UART1", STATE_COMPLETE},
    {STATE_COMPLETE, NULL, "Configurazione completata.", STATE_IDLE}
};

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

    // Inizializza il pin del LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Variabile per il controllo del tempo per il blink del LED
    absolute_time_t led_time = make_timeout_time_ms(500);
    bool led_on = false;
    absolute_time_t command_time = make_timeout_time_ms(2000);  // Prima attesa di 2 secondi
    int at_command_counter = 0;

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

        // Macchina a stati per l'invio di comandi AT
        switch (state) {
            case STATE_WAIT_BEFORE_START:
                // Attendi 2 secondi prima di iniziare
                if (absolute_time_diff_us(get_absolute_time(), command_time) <= 0) {
                    state = STATE_SEND_AT_RST;
                    command_time = make_timeout_time_ms(5000);  // Attesa di 5 secondi dopo il reset
                }
                break;

            case STATE_SEND_AT_RST:
                if (uart_is_writable(uart1)) {
                    printf("Inviando AT+RST alla UART1\n");
                    uart_puts(uart1, "AT+RST\r\n");
                    state = STATE_WAIT_AFTER_RST;
                }
                break;

            case STATE_WAIT_AFTER_RST:
                // Attendi 5 secondi dopo il comando di reset
                if (absolute_time_diff_us(get_absolute_time(), command_time) <= 0) {
                    state = STATE_SEND_AT_GMR;
                    command_time = make_timeout_time_ms(COMMAND_INTERVAL_MS);
                }
                break;

            default:
                for (int i = 0; i < sizeof(at_command_sequence) / sizeof(at_command_t); i++) {
                    if (at_command_sequence[i].state == state) {
                        // Attendi fino a quando non è il momento di inviare il prossimo comando
                        if (absolute_time_diff_us(get_absolute_time(), command_time) <= 0) {
                            if (uart_is_writable(uart1)) {
                                if (at_command_sequence[i].command != NULL) {
                                    at_command_counter++;
                                    // Stampa su UART0 che stai inviando il comando e il numero di richieste
                                    printf("%s, richiesta numero: %d\n", at_command_sequence[i].message, at_command_counter);
                                    uart_puts(uart1, at_command_sequence[i].command);
                                } else {
                                    printf("%s\n", at_command_sequence[i].message);
                                }
                                state = at_command_sequence[i].next_state;
                                command_time = make_timeout_time_ms(COMMAND_INTERVAL_MS);
                            }
                        }
                        break;
                    }
                }
                break;

            case STATE_IDLE:
                // Macchina a stati ferma, non fa nulla
                break;
        }

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
