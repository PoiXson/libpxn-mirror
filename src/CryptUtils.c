#include "CryptUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

#include "Logs.h"



void sha256_string(char *str, char out[SHA256_HASH_SIZE], size_t size) {
	char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str, size);
	SHA256_Final(hash, &sha256);
	int i = 0;
	for(i=0; i<SHA256_DIGEST_LENGTH; i++) {
		sprintf(out + (i * 2), "%02x", hash[i]);
	}
	out[64] = '\0';
}

int sha256_file(char *path, char out[SHA256_HASH_SIZE]) {
	FILE *file = fopen(path, "rb");
	if (!file) return -534;
	char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	char *buf = malloc(bufSize);
	if (buf == NULL) {
		log_severe("Failed to allocate memory for hash");
		return -1;
	}
	int bytesRead = 0;
	while ((bytesRead = fread(buf, 1, bufSize, file))) {
		SHA256_Update(&sha256, buf, bytesRead);
	}
	SHA256_Final(hash, &sha256);
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++) {
		sprintf(out + (i * 2), "%02x", hash[i]);
	}
	out[64] = '\0';
	fclose(file);
	free(buf);
	return 0;
}
