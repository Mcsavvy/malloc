#ifndef MALLOC_DEBUG_H
#define MALLOC_DEBUG_H

#define DEBUG 0

#include <stdarg.h>

void debug(const char *format, ...);
void error(const char *format, ...);

#endif