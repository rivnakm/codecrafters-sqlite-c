#ifndef CC_SQLITE_COMMANDS_DBINFO_H
#define CC_SQLITE_COMMANDS_DBINFO_H

#include <stddef.h>
#include <stdint.h>
#include "cc_sqlite/database.h"

typedef struct {
    uint16_t page_size;
    uint16_t num_tables;
} DbInfo;

void cmd_dbinfo(const Database *db);
int get_dbinfo(const Database *db, DbInfo *info);

#endif
