#include "configuration.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct configuration_parse_context_t
{
    int current_line_number;
    const char *filename;
} configuration_parse_context_t;

static void configuration_print_error(const configuration_parse_context_t *context, const char *message)
{
    printf("osciboot::configuration error: %s:%d: %s\n", context->filename, context->current_line_number, message);
}

static char *configuration_trim_whitespace(char *text)
{
    while (isspace((unsigned char) (*text)) != 0)
    {
        text++;
    }

    char *end_character = text + strlen(text);

    if (end_character != text)
    {
        end_character--;
    }

    while (end_character > text && isspace((unsigned char) (*end_character)) != 0)
    {
        *end_character = '\0';
        end_character--;
    }

    return text;
}

static configuration_result_t configuration_validate_section(
    configuration_parse_context_t *context,
    configuration_entry_t *entry
)
{
    if (entry->executable_path[0] != '\0')
    {
        return CONFIGURATION_RESULT_OK;
    }

    configuration_print_error(context, "section missing required field 'exec'");
    return CONFIGURATION_RESULT_INVALID_SYNTAX;
}

static configuration_result_t configuration_parse_section(
    configuration_t *configuration,
    configuration_parse_context_t *context,
    const char *section_name
)
{
    if (configuration->entries_count > 0)
    {
        configuration_entry_t *previous = &configuration->entries[configuration->entries_count - 1];

        configuration_result_t validation = configuration_validate_section(context, previous);

        if (validation != CONFIGURATION_RESULT_OK)
        {
            return validation;
        }
    }

    if (configuration->entries_count >= CONFIGURATION_MAX_ENTRIES)
    {
        configuration_print_error(context, "too many entries");
        return CONFIGURATION_RESULT_TOO_MANY_ENTRIES;
    }

    configuration_entry_t *entry = &configuration->entries[configuration->entries_count];

    memset(entry, 0, sizeof(configuration_entry_t));

    strncpy(entry->name, section_name, CONFIGURATION_ENTRY_MAX_NAME_LENGTH - 1);
    entry->name[CONFIGURATION_ENTRY_MAX_NAME_LENGTH - 1] = '\0';

    configuration->entries_count++;

    return CONFIGURATION_RESULT_OK;
}

static configuration_result_t configuration_parse_key_value(
    configuration_t *configuration,
    configuration_parse_context_t *context,
    const char *key,
    const char *value
)
{
    if (configuration->entries_count == 0)
    {
        configuration_print_error(context, "key-value pair without section");
        return CONFIGURATION_RESULT_NO_SECTION_DEFINED;
    }

    configuration_entry_t *entry = &configuration->entries[configuration->entries_count - 1];

    if (strcmp(key, "exec") != 0)
    {
        configuration_print_error(context, "unknown key (only 'exec' allowed)");
        return CONFIGURATION_RESULT_INVALID_SYNTAX;
    }

    strncpy(entry->executable_path, value, CONFIGURATION_ENTRY_MAX_EXECUTABLE_PATH_LENGTH - 1);
    entry->executable_path[CONFIGURATION_ENTRY_MAX_EXECUTABLE_PATH_LENGTH - 1] = '\0';

    return CONFIGURATION_RESULT_OK;
}

static configuration_result_t configuration_parse_line(
    configuration_t *configuration,
    configuration_parse_context_t *context,
    char *line
)
{
    context->current_line_number++;

    char *trimmed_line = configuration_trim_whitespace(line);

    if (*trimmed_line == '\0')
    {
        return CONFIGURATION_RESULT_OK;
    }

    if (*trimmed_line == '#')
    {
        return CONFIGURATION_RESULT_OK;
    }

    if (*trimmed_line == '[')
    {
        char *closing_bracket = strchr(trimmed_line, ']');

        if (closing_bracket == NULL)
        {
            configuration_print_error(context, "missing closing bracket ']'");
            return CONFIGURATION_RESULT_INVALID_SYNTAX;
        }

        *closing_bracket = '\0';

        char *section_name = configuration_trim_whitespace(trimmed_line + 1);

        return configuration_parse_section(configuration, context, section_name);
    }

    char *equals_sign = strchr(trimmed_line, '=');

    if (equals_sign == NULL)
    {
        configuration_print_error(context, "missing '=' in key-value pair");
        return CONFIGURATION_RESULT_INVALID_SYNTAX;
    }

    *equals_sign = '\0';

    char *key = configuration_trim_whitespace(trimmed_line);
    char *value = configuration_trim_whitespace(equals_sign + 1);

    return configuration_parse_key_value(configuration, context, key, value);
}

configuration_result_t configuration_load(configuration_t *const configuration, const char *const filename)
{
    FILE *file = fopen(filename, "r");

    configuration_parse_context_t context;
    memset(&context, 0, sizeof(context));
    context.filename = filename;
    context.current_line_number = 0;

    if (file == NULL)
    {
        configuration_print_error(&context, "file not found");
        return CONFIGURATION_RESULT_FILE_NOT_FOUND;
    }

    memset(configuration, 0, sizeof(*configuration));

    char line_buffer[512];

    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL)
    {
        line_buffer[strcspn(line_buffer, "\r\n")] = '\0';

        configuration_result_t result = configuration_parse_line(configuration, &context, line_buffer);

        if (result != CONFIGURATION_RESULT_OK)
        {
            fclose(file);
            return result;
        }
    }

    fclose(file);

    if (configuration->entries_count > 0)
    {
        configuration_entry_t *last = &configuration->entries[configuration->entries_count - 1];

        configuration_result_t result = configuration_validate_section(&context, last);

        if (result != CONFIGURATION_RESULT_OK)
        {
            return result;
        }
    }

    if (configuration->entries_count == 0)
    {
        configuration_print_error(&context, "no sections defined");
        return CONFIGURATION_RESULT_NO_SECTION_DEFINED;
    }

    return CONFIGURATION_RESULT_OK;
}
