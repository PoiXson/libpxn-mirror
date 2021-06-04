#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>



typedef enum File_Type {
	NOT_FOUND = 0x0,
	IS_FILE   = 0x1,
	IS_DIR    = 0x2
} File_Type;

File_Type get_file_type(const char *path);



char* basename(char *file);
void build_path(char *path, const size_t num, ...);

int get_lock(char *lock_file, const bool blocking);
void free_lock(char *lock_file, const int handle);

size_t load_text_file(const char *file, char **out);
size_t read_text_file(FILE *handle, char **out);
bool   save_text_file(const char *file, char *data, size_t size);

bool copy_file(char *file_path, char *dest_path);

size_t file_compare(const char *file, char *data, size_t data_len);
