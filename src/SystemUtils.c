#include "SystemUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>



void assert_user_root() {
	uid_t user_id = getuid();
	if (user_id != 0) {
		printf("\nPermission denied; run as root\n\n");
		exit(EACCES);
	}
}

void assert_not_user_root() {
	uid_t user_id = getuid();
	if (user_id == 0) {
		printf("\nPermission denied; run as a non-root user\n\n");
		exit(EACCES);
	}
}
