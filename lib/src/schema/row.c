#include "cc_sqlite/schema/row.h"

#include <string.h>

SchemaRowType str_to_row_type(const char *str)
{
    SchemaRowType row_type;
    if (strcmp(str, "table") == 0)
    {
        row_type = SCHEMA_ROW_TYPE_TABLE;
    }
    else if (strcmp(str, "index") == 0)
    {
        row_type = SCHEMA_ROW_TYPE_INDEX;
    }
    else if (strcmp(str, "view") == 0)
    {
        row_type = SCHEMA_ROW_TYPE_VIEW;
    }
    else if (strcmp(str, "trigger") == 0)
    {
        row_type = SCHEMA_ROW_TYPE_TRIGGER;
    }

    return row_type;
}
