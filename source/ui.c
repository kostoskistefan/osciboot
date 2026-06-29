#include "ui.h"
#include "font.h"
#include "color.h"
#include "framebuffer.h"

#include <string.h>
#include <stdio.h>

typedef struct ui_message_t
{
    char text[128];
    ui_message_severity_t severity;
} ui_message_t;

typedef struct ui_context_t
{
    framebuffer_t *framebuffer;

    ui_message_t messages[UI_MESSAGE_CAPACITY];

    int message_count;
    int message_write_index;
} ui_context_t;

ui_context_t ui_context;

static uint16_t ui_get_severity_color(ui_message_severity_t severity)
{
    switch (severity)
    {
        case UI_MESSAGE_SEVERITY_INFO:
            return COLOR_BLUE;

        case UI_MESSAGE_SEVERITY_WARNING:
            return COLOR_YELLOW;

        case UI_MESSAGE_SEVERITY_ERROR:
            return COLOR_RED;

        default:
            return COLOR_LIGHT_GRAY;
    }
}

void ui_initialize(framebuffer_t *framebuffer)
{
    ui_context.framebuffer = framebuffer;
    ui_context.message_count = 0;
    ui_context.message_write_index = 0;
}

void ui_push_message(ui_message_severity_t severity, const char *text)
{
    if (text == NULL)
    {
        return;
    }

    int index = ui_context.message_write_index;

    snprintf(ui_context.messages[index].text, sizeof(ui_context.messages[index].text), "%s", text);

    ui_context.messages[index].severity = severity;

    ui_context.message_write_index = (ui_context.message_write_index + 1) % UI_MESSAGE_CAPACITY;

    if (ui_context.message_count < UI_MESSAGE_CAPACITY)
    {
        ui_context.message_count++;
    }
}

void ui_render(void)
{
    if (ui_context.framebuffer == NULL)
    {
        return;
    }

    const font_t *font = &font8x16;

    const int padding = 8;
    const int line_height = font->glyph_height + 2;

    int y = ui_context.framebuffer->visible_height - padding - font->glyph_height;

    int index = ui_context.message_write_index;

    for (int i = 0; i < ui_context.message_count; i++)
    {
        index = (index - 1 + UI_MESSAGE_CAPACITY) % UI_MESSAGE_CAPACITY;

        const ui_message_t *message = &ui_context.messages[index];

        int text_length = (int)strlen(message->text);
        int text_width = text_length * font->glyph_width;

        int x = ui_context.framebuffer->visible_width - text_width - padding;

        uint16_t color = ui_get_severity_color(message->severity);

        framebuffer_fill_rectangle(
            ui_context.framebuffer,
            ui_context.framebuffer->visible_width - (ui_context.framebuffer->visible_width / 4),
            y,
            ui_context.framebuffer->visible_width / 4,
            line_height,
            COLOR_BLACK
        );

        framebuffer_render_text(
            ui_context.framebuffer,
            font,
            x,
            y,
            message->text,
            color
        );

        y -= line_height;

        if (y < 0)
        {
            break;
        }
    }
}
