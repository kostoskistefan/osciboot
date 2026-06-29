#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>

#define UI_MESSAGE_CAPACITY 4

typedef struct font_t font_t;
typedef struct framebuffer_t framebuffer_t;

typedef enum ui_message_severity_t
{
    UI_MESSAGE_SEVERITY_INFO,
    UI_MESSAGE_SEVERITY_WARNING,
    UI_MESSAGE_SEVERITY_ERROR
} ui_message_severity_t;

void ui_initialize(framebuffer_t *framebuffer);
void ui_render(void);
void ui_push_message(ui_message_severity_t severity, const char *text);

#endif
