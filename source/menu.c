#include "menu.h"
#include "font.h"
#include "color.h"
#include "framebuffer.h"
#include "configuration.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MENU_ENTRY_WIDTH  200
#define MENU_ENTRY_HEIGHT 20

void menu_initialize(menu_t *const menu, framebuffer_t *framebuffer, const configuration_t *configuration)
{
    menu->selected_entry_index = 0;
    menu->previous_selected_entry_index = 0;
    menu->framebuffer = framebuffer;
    menu->configuration = configuration;
}

static void menu_render_entry(
    menu_t *const menu,
    const configuration_entry_t *const entry,
    const uint16_t x,
    const uint16_t y,
    bool is_selected
)
{
    const uint16_t color = is_selected ? COLOR_LIGHT_GRAY : COLOR_DARK_GRAY;

    framebuffer_fill_rectangle(menu->framebuffer, x, y, MENU_ENTRY_WIDTH, MENU_ENTRY_HEIGHT, color);
    framebuffer_render_text(menu->framebuffer, &font8x16, x + 4, y + 2, entry->name, COLOR_WHITE);
}

void menu_render(menu_t *const menu, const bool force)
{
    if (menu->selected_entry_index == menu->previous_selected_entry_index && force == false)
    {
        return;
    }

    const uint16_t menu_height = menu->configuration->entries_count * MENU_ENTRY_HEIGHT;

    int32_t x = ((int32_t) menu->framebuffer->visible_width - MENU_ENTRY_WIDTH) / 2;
    int32_t y = ((int32_t) menu->framebuffer->visible_height - menu_height) / 2;

    for (uint8_t i = 0; i < menu->configuration->entries_count; ++i)
    {
        const configuration_entry_t *entry = &menu->configuration->entries[i];
        menu_render_entry(menu, entry, x, y, menu->selected_entry_index == i);
        y += MENU_ENTRY_HEIGHT;
    }
}

void menu_update_selection(menu_t *const menu, const int direction)
{
    menu->previous_selected_entry_index = menu->selected_entry_index;

    menu->selected_entry_index += direction;

    if (menu->selected_entry_index < 0)
    {
        menu->selected_entry_index = menu->configuration->entries_count - 1;
    }

    else if (menu->selected_entry_index >= menu->configuration->entries_count)
    {
        menu->selected_entry_index = 0;
    }
}

void menu_execute(menu_t *menu)
{
    const char *path = menu->configuration->entries[menu->selected_entry_index].executable_path;

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        execv(path, (char *[]) { (char *) path, NULL });
        perror("execv");
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);
}
