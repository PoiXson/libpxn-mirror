#pragma once
#ifdef DEBUG

#include <stdlib.h>
#include <stdbool.h>



#define UNUSED(x) (void)(x)



bool is_mapped(const void *ptr, const size_t bytes);

void hex_dump(void *addr, const size_t len);



#endif
