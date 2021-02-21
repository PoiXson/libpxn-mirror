#include "FileUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>   // getpid
#include <dirent.h>   // DIR
#include <sys/file.h> // open
#include <errno.h>    // errno

#include "StringUtils.h"
#include "Logs.h"



File_Type get_file_type(const char *path) {
	DIR *dir = opendir(path);
	if (dir == NULL)
		return (errno == ENOTDIR ? IS_FILE : NOT_FOUND);
	closedir(dir);
	return IS_DIR;
}



char* basename(char *file) {
	size_t pos = strlen(file) - 1;
	while (true) {
		if (file[pos] == '/') {
			pos++;
			return file + pos;
		}
		if (pos == 0)
			break;
		pos--;
	}
	return file;
}

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



// error codes found at: /usr/include/asm-generic/errno-base.h
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
		return -1;
	fseek(handle, 0, SEEK_END);
	int size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	*out = malloc(size + 1);
	size_t len = fread(*out, 1, size, handle);
	if (len != size) {
		log_warning("read size different from file seek size: %i, %lu", size, len);
	}
	out[size] = '\0';
	return size;
}

bool save_text_file(const char *file, char *data, size_t size) {
	FILE *handle = fopen(file, "w");
	if (handle == NULL)
		return false;
	int len = fwrite(data, size, 1, handle);
	fclose(handle);
	return (len == 1);
}



bool copy_file(char *file_path, char *dest_path) {
	{
		File_Type type = get_file_type(dest_path);
		switch (type) {
		case IS_FILE:
			log_notice("Dest file already exists: %s", dest_path);
			return false;
		case IS_DIR: {
			char path[PATH_MAX+1];
			strlcpy(path, dest_path, PATH_MAX);
			size_t len = strlen(path);
			if (path[len-1] != '/') {
				path[len  ] = '/';
				path[len+1] = '\0';
			}
			strlcat(path, basename(file_path), PATH_MAX);
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
