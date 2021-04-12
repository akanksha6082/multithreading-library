#ifndef __UTILS_H
#define __UTILS_H

#include "athread_types.h"

static size_t stack_limit;
static size_t page_size;

_uint get_stack_limit();

_uint get_page_size();

#endif