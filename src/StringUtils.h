#pragma once

#include <stdbool.h>
#include <stdlib.h> // size_t



size_t strlcpy(char *dest, char *src, const size_t size);
size_t strlcat(char *dest, char *src, const size_t size);

size_t strlrcpy(char **dest, char *src, size_t *size);
size_t strlrcat(char **dest, char *src, size_t *size);

int strlcmp(const char *strA, const char *strB, const size_t size);

char* str_trim(char *str);

size_t chrpos(const char *haystack, const char needle);
size_t chrposs(const char *haystack, const char needle, const size_t start);

void str_pad_front(char *line, const size_t size);
void str_pad_end(char *line, const size_t size);
void str_pad_center(char *line, const size_t size);

bool str_starts_with(const char *haystack, const char *needle);
bool str_ends_with(const char *haystack, const char *needle);
