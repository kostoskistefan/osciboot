#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct framebuffer_t framebuffer_t;
typedef struct configuration_t configuration_t;

typedef struct menu_t
{
    int8_t previous_selected_entry_index;
    int8_t selected_entry_index;

    framebuffer_t *framebuffer;
    const configuration_t *configuration;
} menu_t;

void menu_initialize(menu_t *const menu, framebuffer_t *framebuffer, const configuration_t *configuration);
void menu_render(menu_t *const menu, const bool force);
void menu_update_selection(menu_t *const menu, const int direction);
void menu_execute(menu_t *const menu);

#endif // MENU_H
