#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc_sqlite.h"
#include "cc_sqlite/database.h"
#include "cc_sqlite/page_header.h"
#include "cc_sqlite/record.h"
#include "cc_sqlite/schema/row.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <database path> <command>\n", argv[0]);
        return 1;
    }

    const char *database_file_path = argv[1];
    const char *command = argv[2];

    FILE *database_file = fopen(database_file_path, "rb");
    if (!database_file)
    {
        fprintf(stderr, "Failed to open the database file\n");
        return 1;
    }

    Database db;
    int err = db_open(database_file, &db) != EXIT_SUCCESS;
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to load database\n");
        return err;
    }

    if (strcmp(command, ".dbinfo") == 0)
    {
        PageHeader page_header;
        err = db_get_page_header(&db, 0, &page_header);
        if (err != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to load database\n");
            return err;
        }
        printf("database page size: %u\n", db.file_header.page_size);
        printf("number of tables: %u\n", page_header.cell_count);
    }
    else if (strcmp(command, ".tables") == 0)
    {
        char **names = NULL;
        size_t count;
        err = db_get_table_names(&db, &names, &count);
        if (err != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read table names\n");
            return err;
        }

        for (size_t i = 0; i < count; i++)
        {
            if (i < count - 1)
            {
                printf("%s ", names[i]);
            }
            else
            {
                printf("%s\n", names[i]);
            }
            free(names[i]);
        }
        free(names);
    }
    else
    {
        // assume "SELECT COUNT(*) FROM tablename"
        char *sql_command = (char *)malloc(strlen(command) + 1);
        strcpy(sql_command, command);

        char *token = strtok(sql_command, " ");
        for (size_t i = 0; i < 3; i++)
        {
            token = strtok(NULL, " ");
        }

        SchemaRow row;
        err = db_get_table_by_name(&db, token, &row);
        if (err != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to find table by name\n");
            return err;
        }

        free(sql_command);

        PageHeader page_header;
        err = db_get_page_header(&db, row.rootpage - 1, &page_header);
        if (err != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read page header %ld\n", row.rootpage);
            return err;
        }

        Record *records = NULL;
        size_t records_count = 0;
        err = db_get_table_records(&db, &page_header, &records, &records_count);
        if (err != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read table records\n");
            return err;
        }

        printf("%zu\n", records_count);

        for (size_t i = 0; i < records_count; i++)
        {
            for (size_t j = 0; j < records[i].header.count; j++)
            {
                record_data_free(records[i].columns[i].data);
                records[i].columns[i].data = NULL;
            }
        }
        records = NULL;
    }

    fclose(database_file);

    return 0;
}
