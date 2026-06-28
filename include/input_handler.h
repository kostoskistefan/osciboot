#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <linux/input.h>

typedef struct
{
    int file_descriptor;
    struct input_event event;
} input_handler_t;

void input_handler_initialize(input_handler_t *const input_handler, const char *device);
void input_handler_destroy(input_handler_t *input_handler);
int input_handler_read(input_handler_t *input_handler);

#endif // INPUT_HANDLER_H
