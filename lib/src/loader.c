#include "cc_sqlite/loader.h"

#include <stdlib.h>

int db_open(FILE *file, Database *db)
{
    db->file = file;
    if (file_header_read(file, &db->file_header) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to read file header\n");
        return EXIT_FAILURE;
    }

    if (page_header_read(file, db->file_header.page_size, 0, &db->page_header) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to read page header\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}