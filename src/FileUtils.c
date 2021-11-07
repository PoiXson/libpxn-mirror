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
#include "FileUtils.h"

#include "StringUtils.h"
#include "MemUtils.h"

#include <string.h>
#include <dirent.h>   // DIR
#include <sys/file.h> // open
#include <errno.h>    // errno



File_Type get_file_type(const char *path) {
	DIR *dir = opendir(path);
	if (dir == NULL)
		return (errno == ENOTDIR ? IS_FILE : NOT_FOUND);
	closedir(dir);
	return IS_DIR;
}



char* get_basename(char *file) {
	size_t pos = str_len(file);
	if (pos == 0) return file;
	while (true) {
		pos--;
		if (file[pos] == '/')
			return file + pos + 1;
		if (pos == 0)
			break;
	}
	return file;
}



char* build_path(const size_t num_parts, ...) {
//TODO
return NULL;
}
/*
void build_path(char *path, const size_t num, ...) {
	memset(path, '\0', PATH_MAX);
	if (num == 0)
		return;
	va_list args;
	va_start(args, num);
	bool is_abs = false;
	bool is_dir = false;
	char *arg;
	size_t len;
	for (size_t arg_index=0; arg_index<num; arg_index++) {
		arg = va_arg(args, char*);
		// first part
		if (arg_index == 0) {
			if (arg[0] == '/') {
				is_abs = true;
			}
		}
		// trim / from front
		while (true) {
			if (arg[0] == '\0')
				break;
			if(arg[0] != '/')
				break;
			arg++;
		}
		// append to result
		if (arg_index == 0) {
			if (is_abs) {
				path[0] = '/';
				path[1] = '\0';
			}
		} else {
			len = strlen(path);
			if (len >= PATH_MAX)
				len = PATH_MAX - 1;
			path[len  ] = '/';
			path[len+1] = '\0';
		}
		strlcat(path, arg, PATH_MAX);
		// trim / from end
		while (true) {
			len = strlen(path);
			if (len == 0)
				break;
			if (path[len-1] != '/')
				break;
			path[len-1] = '\0';
		}
	}
	if (is_dir) {
		len = strlen(path);
		if (len >= PATH_MAX)
			len = PATH_MAX - 1;
		path[len  ] = '/';
		path[len+1] = '\0';
	}
	va_end(args);
}
*/



// ========================================
// load/save file



size_t load_text_file(const char *file, char **out) {
	FILE *handle = fopen(file, "r");
	if (handle == NULL)
		return -1L;
	size_t size = read_text_file(handle, out);
	fclose(handle);
	return size;
}

size_t read_text_file(FILE *handle, char **out) {
	if (handle == NULL)
		return -1L;
	size_t chunk_size = 256L;
	size_t size = 0L;
	char chunk[chunk_size];
	int len;
	size_t pos = 0;
	while (!feof(handle)) {
		len = fread(&chunk, 1L, chunk_size, handle);
		if (len <= 0)
			break;
		if (size == 0L) {
			size = len + 1;
			*out = calloc(size, 1);
			memcpy(*out, chunk, len);
			pos += len;
			(*out)[pos] = '\0';
		} else {
			if (pos + len + 1 > size) {
				size_t old_size = size;
				size_t new_size = size * 2;
				*out = realloc_zero(*out, old_size, new_size);
			}
			memcpy( (*out)+pos, chunk, len);
			pos += len;
			(*out)[pos] = '\0';
		}
	}
	return size;
}



bool save_text_file(const char *file, char *data, size_t size) {
	FILE *handle = fopen(file, "w");
	if (handle == NULL)
		return false;
	int len = fwrite(data, size, 1, handle);
	fclose(handle);
//TODO: is this right?
	return (len == 1);
}



//TODO: redo this
/*
bool copy_file(char *file_path, char *dest_path) {
	{
		File_Type type = get_file_type(dest_path);
		switch (type) {
		case IS_FILE:
			log_notice("Dest file already exists: %s", dest_path);
			return false;
		case IS_DIR: {
			char path[PATH_MAX+1];
			str_l_cpy(path, dest_path, PATH_MAX);
			size_t len = strlen(path);
			if (path[len-1] != '/') {
				path[len  ] = '/';
				path[len+1] = '\0';
			}
			str_l_cat(path, basename(file_path), PATH_MAX);
			return copy_file(file_path, path);
		}
		default:
			break;
		}
	}
	int handle_source = open(file_path, O_RDONLY);
	if (handle_source < 0) {
		log_notice("Failed to open source file: %s", file_path);
		return false;
	}
	int handle_dest = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if (handle_dest < 0) {
		log_notice("Failed to write file: %s", dest_path);
		close(handle_source);
		return false;
	}
	char buffer[4096];
	size_t len;
	while (true) {
		len = read(handle_source, buffer, sizeof(buffer));
		if (len <= 0)
			break;
		char *ptr = buffer;
		size_t written;
		while (len > 0) {
			written = write(handle_dest, ptr, len);
			if (written < 0) {
				if (errno == EINTR)
					continue;
				log_notice("Failed to write file: %s", dest_path);
				return false;
			}
			len -= written;
			ptr += written;
		}
	}
	if (len == 0) {
		close(handle_source);
		if (close(handle_dest) < 0) {
			log_notice("Failed closing written file: %s", dest_path);
			return false;
		}
	}
	return true;
}
*/



//TODO: redo this
/*
size_t file_compare(const char *file, char *data, size_t data_len) {
	if (IS_FILE != get_file_type(file))
		return -1;
	FILE *handle = fopen(file, "r");
	if (handle == NULL)
		return -1;
	size_t pos = 0;
	char chr;
	while (!feof(handle)) {
		if (pos > data_len) {
			while (!feof(handle)) {
				chr = fgetc(handle);
				if (chr == -1)
					break;
				if (chr == '\n') continue;
				if (chr == '\r') continue;
			}
			fclose(handle);
			return 0;
		}
		chr = fgetc(handle);
		if (chr == -1)
			break;
		if (chr == '\0' && data[pos] == '\0') {
			fclose(handle);
			return 0; // match
		}
		if (chr == '\0' || data[pos] == '\0') {
			fclose(handle);
			if (pos == 0) pos++;
			return pos; // differ
		}
		if (chr != data[pos]) {
			fclose(handle);
			if (pos == 0) pos++;
			return pos; // differ
		}
		pos++;
	}
	fclose(handle);
	if (data_len > pos) {
		if (data[pos+1] == '\0')
			return 0;
		while (true) {
			pos++;
			if (pos > data_len) return 0;
			if (data[pos] == '\0') return 0;
			if (data[pos] == '\n') continue;
			if (data[pos] == '\r') continue;
			break;
		}
		return pos+1;
	}
	return 0;
}
*/



// ========================================
// lock file



// note: error codes found at: /usr/include/asm-generic/errno-base.h
/*
int get_lock(char *lock_file, const bool blocking) {
	int handle = open(lock_file, O_CREAT | O_RDWR, 0666);
	if (handle < 0) {
		if (EACCES == errno) {
			log_fatal("Permission denied to lock file: %s", lock_file);
		} else
		if (EIO == errno) {
			log_fatal("IO Error accessing file: %s", lock_file);
		} else
		if (ENOSPC == errno) {
			log_fatal("No space left on device for file: %s", lock_file);
		} else {
			log_fatal("Failed to open lock file: %s errno: %i", lock_file, errno);
		}
		exit(1);
	}
	if (flock(handle, LOCK_EX | (blocking ? 0 : LOCK_NB) )) {
		if (EWOULDBLOCK == errno) {
			log_fatal("Concurrent instance, already running");
		} else {
			log_fatal("Failed to lock file: %s errno: %i", lock_file, errno);
		}
		exit(1);
	}
	int pid = getpid();
	dprintf(handle, "%i", pid);
	log_detail("Got instance lock for pid: %i", pid);
	return handle;
}

void free_lock(char *lock_file, const int handle) {
	close(handle);
	if (remove(lock_file)) {
		log_warning("Failed to remove lock file: %s", lock_file);
	} else {
		log_detail("Released lock file: %s", lock_file);
	}
}
*/
