#include "FileUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>   // getpid
#include <dirent.h>   // DIR
#include <sys/file.h> // open
#include <errno.h>    // errno

#include "StringUtils.h"



File_Type get_file_type(const char *path) {
	DIR *dir = opendir(path);
	if (dir == NULL)
		return (errno == ENOTDIR ? is_file : not_found);
	closedir(dir);
	return is_dir;
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



// error codes found at: /usr/include/asm-generic/errno-base.h
int get_lock(char *lock_file, const bool blocking) {
	int handle = open(lock_file, O_CREAT | O_RDWR, 0666);
	if (handle < 0) {
		if (EACCES == errno) {
//			log_fatal("Permission denied to lock file: %s", lock_file);
		} else
		if (EIO == errno) {
//			log_fatal("IO Error accessing file: %s", lock_file);
		} else
		if (ENOSPC == errno) {
//			log_fatal("No space left on device for file: %s", lock_file);
		} else {
//			log_fatal("Failed to open lock file: %s errno: %i", lock_file, errno);
		}
		exit(1);
	}
	if (flock(handle, LOCK_EX | (blocking ? 0 : LOCK_NB) )) {
		if (EWOULDBLOCK == errno) {
//			log_fatal("Concurrent instance, already running");
		} else {
//			log_fatal("Failed to lock file: %s errno: %i", lock_file, errno);
		}
		exit(1);
	}
	int pid = getpid();
	dprintf(handle, "%i", pid);
//	log_detail("Got instance lock for pid: %i", pid);
	return handle;
}

void free_lock(char *lock_file, const int handle) {
	close(handle);
	if (remove(lock_file)) {
//		log_warning("Failed to remove lock file: %s", lock_file);
	} else {
//		log_detail("Released lock file: %s", lock_file);
	}
}



int load_text_file(char *file, char **out) {
	FILE *handle = fopen(file, "r");
	if (handle == NULL)
		return -1;
	int len = read_text_file(handle, out);
	fclose(handle);
	return len;
}

int read_text_file(FILE *handle, char **out) {
	if (handle == NULL)
		return -1;
	fseek(handle, 0, SEEK_END);
	int size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	*out = malloc(size + 1);
	size_t len = fread(*out, 1, size, handle);
	if (len != size) {
//		log_warning("read size different from file seek size: %i, %lu", size, len);
	}
	out[size] = '\0';
	return size;
}

bool save_text_file(char *file, char *data, size_t size) {
	FILE *handle = fopen(file, "w");
	if (handle == NULL)
		return false;
	size_t len = fwrite(data, size, 1, handle);
	fclose(handle);
	return (len ==1);
}



bool copy_file(char *file_path, char *dest_path) {
	{
		File_Type type = get_file_type(dest_path);
		switch (type) {
		case is_file:
//			log_notice("Dest file already exists: %s", dest_path);
			return false;
		case is_dir: {
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
//		log_notice("Failed to open source file: %s", file_path);
		return false;
	}
	int handle_dest = open(dest_path, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if (handle_dest < 0) {
//		log_notice("Failed to write file: %s", dest_path);
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
//				log_notice("Failed to write file: %s", dest_path);
				return false;
			}
			len -= written;
			ptr += written;
		}
	}
	if (len == 0) {
		close(handle_source);
		if (close(handle_dest) < 0) {
//			log_notice("Failed closing written file: %s", dest_path);
			return false;
		}
	}
	return true;
}
