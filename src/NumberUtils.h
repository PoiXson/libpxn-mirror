#pragma once

#include <stdlib.h>



#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))



char* itostr(int value);
size_t len_int(const int value);
