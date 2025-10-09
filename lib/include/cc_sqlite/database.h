#ifndef CCSQLITE_DATABASE_H
#define CCSQLITE_DATABASE_H

#include <stdint.h>

#include "cc_sqlite/file_header.h"
#include "cc_sqlite/page_header.h"

typedef struct
{
    uint16_t page_size;
    uint16_t cell_count;
} DatabaseInfo;

typedef struct
{
    FILE *file;
    FileHeader file_header;
    PageHeader page_header;
} Database;

DatabaseInfo get_db_info(const Database *db);
int get_db_table_names(const Database *db, char **names[], size_t *count);

#endif