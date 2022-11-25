#include "include/log.h"

void mlogf(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

void mwarnf(const char* msg, ...)
{
    printf("[WARNING] ");

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf(" Press enter to continue.");
    char c = getchar();
}

void merrf(const char* msg, ...)
{
    printf("[ERROR] ");

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    
    printf("\n");
    exit(1);
}

u_int64_t mtime_us()
{
    static struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
}

u_int64_t mtime_s()
{
    static struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
