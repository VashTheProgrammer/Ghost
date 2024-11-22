#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "md.h"

// Funzione per codificare in Base64
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

// Funzione per generare il SAS Token
int generate_sas_token(const char *device_id, const char *hostname, const char *key, char *sas_token, size_t sas_token_len) {
    time_t expiry = time(NULL) + 3600; // Il token scade tra 1 ora
    char string_to_sign[256];
    snprintf(string_to_sign, sizeof(string_to_sign), "%s/devices/%s\n%ld", hostname, device_id, expiry);

    unsigned char hmac_output[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)key, strlen(key));
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)string_to_sign, strlen(string_to_sign));
    mbedtls_md_hmac_finish(&ctx, hmac_output);
    mbedtls_md_free(&ctx);

    // Codifica la firma in Base64
    char base64_signature[128];
    base64_encode(hmac_output, sizeof(hmac_output), base64_signature);

    // Costruisci il SAS token
    snprintf(sas_token, sas_token_len, "SharedAccessSignature sr=%s/devices/%s&sig=%s&se=%ld", hostname, device_id, base64_signature, expiry);

    return 1; // Successo
}

