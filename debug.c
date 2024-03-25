#include "debug.h"
#include <stdio.h>


/** 
 * print debug information
 * @param format: the format string
 * @param ...: variadic arguments
 */
void debug(const char *format, ...)
{
    va_list args;
    if (!DEBUG)
        return;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/**
 * print error information
 * @param format: the format string
 * @param ...: variadic arguments
*/
void error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    vfprintf(stderr, format, args);
    va_end(args);
}