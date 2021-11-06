//===============================================================================
// Copyright (c) 2020-2021 PoiXson, Mattsoft
// <https://poixson.com> <https://mattsoft.net>
// Released under the GPL 3.0
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//===============================================================================
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
