#ifndef SAS_TOKEN_GENERATOR_H
#define SAS_TOKEN_GENERATOR_H

#include <stddef.h>

void generate_sas_token(const char *key, const char *resource_uri, int expiry_seconds, char *sas_token, size_t sas_token_size);

#endif // SAS_TOKEN_GENERATOR_H
