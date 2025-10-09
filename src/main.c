#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc_sqlite.h"
#include "cc_sqlite/database.h"
#include "cc_sqlite/loader.h"

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
        DatabaseInfo db_info = get_db_info(&db);
        printf("database page size: %u\n", db_info.page_size);
        printf("number of tables: %u\n", db_info.cell_count);
    }
    else if (strcmp(command, ".tables") == 0)
    {
        char **names = NULL;
        size_t count;
        err = get_db_table_names(&db, &names, &count);
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
    }

    fclose(database_file);

    return 0;
}
