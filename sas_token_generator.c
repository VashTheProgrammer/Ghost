#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"

// Implementazione migliorata di HMAC-SHA256 (funzione simulata)
void improved_hmac_sha256(const char *key, const char *message, unsigned char *output) {
    // Implementazione simulata di HMAC-SHA256 per avvicinarsi al risultato atteso
    size_t key_len = strlen(key);
    size_t message_len = strlen(message);
    for (int i = 0; i < 32; i++) {
        output[i] = (unsigned char)((key[i % key_len] + message[i % message_len]) ^ 0x5c);
    }
}

// Funzione per codificare in Base64 (implementazione semplificata)
void base64_encode(const unsigned char *input, size_t len, char *output) {
    const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                output[j++] = base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        int k;
        for (k = i; k < 3; k++) {
            char_array_3[k] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (k = 0; k < i + 1; k++) {
            output[j++] = base64_chars[char_array_4[k]];
        }

        while (i++ < 3) {
            output[j++] = '=';
        }
    }

    output[j] = '\0';
}

// Funzione per eseguire l'URL encoding
void url_encode(const char *input, char *output, size_t output_size) {
    const char *hex = "0123456789ABCDEF";
    size_t i, j;
    for (i = 0, j = 0; i < strlen(input) && j < output_size - 1; i++) {
        if (('a' <= input[i] && input[i] <= 'z') || ('A' <= input[i] && input[i] <= 'Z') || ('0' <= input[i] && input[i] <= '9') || input[i] == '-' || input[i] == '_' || input[i] == '.' || input[i] == '~') {
            output[j++] = input[i];
        } else {
            if (j + 3 < output_size - 1) {
                output[j++] = '%';
                output[j++] = hex[(input[i] >> 4) & 0xF];
                output[j++] = hex[input[i] & 0xF];
            }
        }
    }
    output[j] = '\0';
}


// Funzione per generare un SAS token senza librerie esterne
void generate_sas_token(const char *key, const char *resource_uri, int expiry_seconds, char *sas_token, size_t sas_token_size) {
    printf("Generazione del SAS Token...\n");
    char string_to_sign[512];
    char base64_signature[512];
    unsigned char hmac_result[64];
    time_t expiry = time(NULL) + expiry_seconds;

    // Crea la stringa da firmare nel formato richiesto da Azure
    snprintf(string_to_sign, sizeof(string_to_sign), "%s\n%ld", resource_uri, expiry);

    // Calcola l'HMAC-SHA256 (simulazione migliorata)
    improved_hmac_sha256(key, string_to_sign, hmac_result);

    // Codifica in Base64 la firma
    base64_encode(hmac_result, sizeof(hmac_result), base64_signature);

    // Esegui l'URL encoding della firma Base64
    char encoded_signature[512];
    url_encode(base64_signature, encoded_signature, sizeof(encoded_signature));

    // Crea il SAS token nel formato richiesto da Azure
    snprintf(sas_token, sas_token_size, "SharedAccessSignature sig=%s&se=%ld&sr=%s", encoded_signature, expiry, resource_uri);
    printf("SAS Token generato: %s\n", sas_token);
}