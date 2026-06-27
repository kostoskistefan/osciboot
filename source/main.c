#include "font.h"
#include "configuration.h"
#include "framebuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int validate_arguments(int argc)
{
    if (argc != 2)
    {
        printf("osciboot: invalid arguments (usage: osciboot <config_file>)\n");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (validate_arguments(argc) == 0)
    {
        return EXIT_FAILURE;
    }

    configuration_t configuration;

    if (configuration_load(&configuration, argv[1]) != CONFIGURATION_RESULT_OK)
    {
        return EXIT_FAILURE;
    }

    framebuffer_t framebuffer;
    framebuffer_create(&framebuffer);

    framebuffer_fill(&framebuffer, 0x000000);

    framebuffer_render_text(
        &framebuffer,
        &font8x16,
        20,
        20,
        "Osciboot ready",
        0x00FF00
    );

    sleep(10);

    framebuffer_destroy(&framebuffer);

    return EXIT_SUCCESS;
}
