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
#include "Debug.h"
#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>



bool is_mapped(const void *ptr, const size_t bytes) {
	if (ptr == NULL)
		return false;
	int fd[2];
	if (pipe(fd) < 0)
		return false;
	if (write(fd[1], ptr, bytes) < 0) {
		if (errno == EFAULT) {
			close(fd[0]); close(fd[1]);
			return false;
		}
	}
	close(fd[0]); close(fd[1]);
	return true;
}



void hex_dump(void *addr, const size_t len) {
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*) addr;
	// process bytes
	size_t i;
	for (i=0; i<len; i++) {
		// multiple of 16 means new line (with line offset)
		if ((i % 16) == 0) {
			// don't print ASCII for the zeroth line
			if (i != 0)
				printf("  %s\n", buff);
			// offset
			printf("  %04x ", (int)i);
		}
		// display in hex
		printf(" %02x", pc[i]);
		// store a printable ASCII character
		if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
			buff[i % 16] = '.';
		} else {
			buff[i % 16] = pc[i];
		}
		buff[(i % 16) + 1] = '\0';
	}
	// pad last line
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}
	// print the final ASCII bit
	printf("  %s\n", buff);
}



#endif
