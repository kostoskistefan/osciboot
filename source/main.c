#include "color.h"
#include "font.h"
#include "configuration.h"
#include "framebuffer.h"
#include "input_handler.h"
#include "menu.h"
#include "keys.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef OSCIBOOT_DEBUG_TTY
#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#endif

#define FRAMEBUFFER_FILE "/dev/fb0"
#define KEYBOARD_FILE    "/dev/input/event7"

static bool validate_arguments(int arguments_count);
static void render_background(framebuffer_t *framebuffer);
static void run(framebuffer_t *framebuffer, menu_t *menu, input_handler_t *input_handler);

#ifdef OSCIBOOT_DEBUG_TTY
static int initialize_debug_tty(void);
static void destroy_debug_tty(int tty_file_descriptor);
#endif

int main(int arguments_count, char *arguments[])
{
    if (validate_arguments(arguments_count) == false)
    {
        return EXIT_FAILURE;
    }

    configuration_t configuration;

    if (configuration_load(&configuration, arguments[1]) != CONFIGURATION_RESULT_OK)
    {
        return EXIT_FAILURE;
    }

#ifdef OSCIBOOT_DEBUG_TTY
    int tty_file_descriptor = initialize_debug_tty();

    if (tty_file_descriptor < 0)
    {
        return EXIT_FAILURE;
    }
#endif

    framebuffer_t framebuffer;
    framebuffer_create(&framebuffer, FRAMEBUFFER_FILE);
    render_background(&framebuffer);

    menu_t menu;
    menu_initialize(&menu, &framebuffer, &configuration);
    menu_render(&menu, true);

    input_handler_t input_handler;
    input_handler_initialize(&input_handler, KEYBOARD_FILE);

    run(&framebuffer, &menu, &input_handler);

    input_handler_destroy(&input_handler);
    framebuffer_destroy(&framebuffer);

#ifdef OSCIBOOT_DEBUG_TTY
    destroy_debug_tty(tty_file_descriptor);
#endif

    return EXIT_SUCCESS;
}

static bool validate_arguments(int arguments_count)
{
    if (arguments_count != 2)
    {
        printf("osciboot: invalid arguments (usage: osciboot <config_file>)\n");
        return false;
    }

    return true;
}

static void render_background(framebuffer_t *framebuffer)
{
    framebuffer_fill(framebuffer, COLOR_BLACK);
    framebuffer_render_text(framebuffer, &font8x16, 8, 456, "osciboot [v1.0.0]", COLOR_LIGHT_GRAY);
}

static void run(framebuffer_t *framebuffer, menu_t *menu, input_handler_t *input_handler)
{
    static const struct timespec update_period = { 0, 16 * 1000 * 1000 };

    bool is_running = true;

    while (is_running)
    {
        switch (input_handler_read(input_handler))
        {
            case KEY_ESC:
                is_running = false;
                break;

            case KEY_UP:
                menu_update_selection(menu, -1);
                break;

            case KEY_DOWN:
                menu_update_selection(menu, 1);
                break;

            case KEY_ENTER:
                menu_execute(menu);
                render_background(framebuffer);
                menu_render(menu, true);
                break;

            default:
                break;
        }

        menu_render(menu, false);

        nanosleep(&update_period, NULL);
    }
}

#ifdef OSCIBOOT_DEBUG_TTY
static int initialize_debug_tty(void)
{
    int tty_file_descriptor = open("/dev/tty2", O_RDWR);

    if (tty_file_descriptor < 0)
    {
        perror("open tty");
        return -1;
    }

    if (ioctl(tty_file_descriptor, KDSETMODE, KD_GRAPHICS) < 0)
    {
        perror("KDSETMODE");
    }

    return tty_file_descriptor;
}

static void destroy_debug_tty(int tty_file_descriptor)
{
    ioctl(tty_file_descriptor, KDSETMODE, KD_TEXT);
    close(tty_file_descriptor);
}
#endif
