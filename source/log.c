#include "log.h"
#include <stdio.h>

void log_info(const char *message)
{
    fprintf(stdout, "[osciboot::info] %s\n", message);
}

void log_warning(const char *message)
{
    fprintf(stdout, "[osciboot::warning] %s\n", message);
}

void log_error(const char *message)
{
    fprintf(stderr, "[osciboot::error] %s\n", message);
}
