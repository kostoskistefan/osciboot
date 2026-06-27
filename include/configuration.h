#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>

#define CONFIGURATION_ENTRY_MAX_NAME_LENGTH            64
#define CONFIGURATION_ENTRY_MAX_EXECUTABLE_PATH_LENGTH 256

#define CONFIGURATION_MAX_ENTRIES 10

typedef struct configuration_entry_t
{
    char name[CONFIGURATION_ENTRY_MAX_NAME_LENGTH];
    char executable_path[CONFIGURATION_ENTRY_MAX_EXECUTABLE_PATH_LENGTH];
} configuration_entry_t;

typedef struct configuration_t
{
    uint8_t entries_count;
    configuration_entry_t entries[CONFIGURATION_MAX_ENTRIES];
} configuration_t;

typedef enum configuration_result_t
{
    CONFIGURATION_RESULT_OK,
    CONFIGURATION_RESULT_FILE_NOT_FOUND,
    CONFIGURATION_RESULT_INVALID_SYNTAX,
    CONFIGURATION_RESULT_TOO_MANY_ENTRIES,
    CONFIGURATION_RESULT_NO_SECTION_DEFINED
} configuration_result_t;

configuration_result_t configuration_load(configuration_t *const configuration, const char *const filename);

#endif // CONFIGURATION_H
