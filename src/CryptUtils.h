#pragma once

#include <stdlib.h>

#include <openssl/sha.h>



#define SHA256_HASH_SIZE ((SHA256_DIGEST_LENGTH * 2) + 1)



void sha256_string(char *str, char out[SHA256_HASH_SIZE], size_t size);
int  sha256_file(char *path, char out[SHA256_HASH_SIZE]);
