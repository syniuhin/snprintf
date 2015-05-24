#ifndef __SNPRINTF_H__
#define __SNPRINTF_H__


#include <stdarg.h>
#include <stdlib.h>

int parse(const char* argb, char* flag, int* width,
        int* precision, char* type);
int my_snprintf(char* s, size_t n, const char* format, ...);

int parse_for_flags(const char* argb);
char* parse_for_width(const char* argb, char* container);
char* parse_for_precision(const char* argb, char* container);
char parse_for_type(const char* argb);
char* from_args(char* to, va_list args, char flag,
        int width, int precision, char type);

#endif
