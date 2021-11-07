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
#include "SystemUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>



bool is_root_user() {
	uid_t user_id = getuid();
	return (user_id == 0);
}

void assert_user_root() {
	if (is_root_user() != true) {
		printf("\nPermission denied; run as root\n\n");
		exit(EACCES);
	}
}

void assert_not_user_root() {
	if (is_root_user()) {
		printf("\nPermission denied; run as a non-root user\n\n");
		exit(EACCES);
	}
}
