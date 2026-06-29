#include "input_handler.h"
#include "log.h"
#include "ui.h"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

void input_handler_initialize(input_handler_t *const input_handler, const char *device)
{
    input_handler->file_descriptor = open(device, O_RDONLY | O_NONBLOCK);

    if (input_handler->file_descriptor < 0)
    {
        log_error("Failed to open input device");
        ui_push_message(UI_MESSAGE_SEVERITY_ERROR, "Failed to open input device");
        return;
    }
    //
    // if (ioctl(input_handler->file_descriptor, EVIOCGRAB, 1) < 0)
    // {
    //     log_error("Failed to get EVIOCGRAB");
    //     ui_push_message(UI_MESSAGE_SEVERITY_ERROR, "Failed to get EVIOCGRAB");
    // }
}

void input_handler_destroy(input_handler_t *const input_handler)
{
    if (input_handler->file_descriptor >= 0)
    {
        // ioctl(input_handler->file_descriptor, EVIOCGRAB, 0);
        close(input_handler->file_descriptor);
    }
}

int input_handler_read(input_handler_t *const input_handler)
{
    if (input_handler->file_descriptor < 0)
    {
        return 0;
    }

    while (
        read(
            input_handler->file_descriptor,
            &input_handler->event,
            sizeof(input_handler->event
        )) == sizeof(input_handler->event))
    {
        if (input_handler->event.type == EV_KEY && input_handler->event.value == 1)
        {
            return input_handler->event.code;
        }
    }

    return 0;
}
