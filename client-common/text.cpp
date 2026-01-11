#include <stdarg.h>
#include <stdio.h>

char status_line[256];
char status_line2[256];

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    if (!l)
        vsprintf(status_line, format, args);
    else
        vsprintf(status_line2, format, args);
    va_end(args);
}

int CONSOLE_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return 0;
}
