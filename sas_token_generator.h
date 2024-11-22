#ifndef SAS_TOKEN_GENERATOR_H
#define SAS_TOKEN_GENERATOR_H

int generate_sas_token(const char *device_id, const char *hostname, const char *key, char *sas_token, size_t sas_token_len);

#endif // SAS_TOKEN_GENERATOR_H
