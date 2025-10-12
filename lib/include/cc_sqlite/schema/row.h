#ifndef CCSQLITE_SCHEMA_ROW_H
#define CCSQLITE_SCHEMA_ROW_H

#include <stdint.h>

typedef enum
{
    SCHEMA_ROW_TYPE_TABLE,
    SCHEMA_ROW_TYPE_INDEX,
    SCHEMA_ROW_TYPE_VIEW,
    SCHEMA_ROW_TYPE_TRIGGER,
} SchemaRowType;

typedef struct
{
    SchemaRowType type;
    char *name;
    char *table_name;
    int64_t rootpage;
} SchemaRow;

/**
 * Convert string to SchemaRowType
 *
 * @param str String to convert
 * @return SchemaRowType
 */
SchemaRowType str_to_row_type(const char *str);

/**
 * Free heap allocated struct members
 *
 * @param row Schema row
 */
void schema_row_free(SchemaRow *row);

#endif
