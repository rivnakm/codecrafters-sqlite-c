#include "cc_sqlite/commands/dbinfo.h"

#include <stdlib.h>

void cmd_dbinfo(const Database *db)
{
    DbInfo info;
    int err = get_dbinfo(db, &info);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to load database\n");
        exit(1);
    }

    printf("database page size: %u\n", info.page_size);
    printf("number of tables: %u\n", info.num_tables);
}

int get_dbinfo(const Database *db, DbInfo *info)
{
    PageHeader page_header;
    int err = db_get_page_header(db, 0, &page_header);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to load database\n");
        return err;
    }

    info->page_size = db->file_header.page_size;
    info->num_tables = page_header.cell_count;

    return EXIT_SUCCESS;
}
