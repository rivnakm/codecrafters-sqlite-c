#include "cc_sqlite/database.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc_sqlite/bytes.h"
#include "cc_sqlite/cell.h"
#include "cc_sqlite/record.h"

DatabaseInfo get_db_info(const Database *db)
{
    DatabaseInfo db_info = {.page_size = db->file_header.page_size, .cell_count = db->page_header.cell_count};

    return db_info;
}

int get_db_table_names(const Database *db, char **names[], size_t *count)
{
    *count = 0;

    uint16_t usable_page_size =
        db->file_header.page_size - db->file_header.reserved - 100; // first page has the db file header

    // cell pointers come right after the page header
    // file header is 100 bytes on page 1 only, otherwise 0
    size_t cell_pointers_offset = 100;
    if (db->page_header.type == PAGE_TYPE_INTERIOR_TABLE || db->page_header.type == PAGE_TYPE_INTERIOR_INDEX)
    {
        cell_pointers_offset += 12;
    }
    else
    {
        cell_pointers_offset += 8;
    }
    fseek(db->file, cell_pointers_offset, SEEK_SET);
    uint16_t *cell_pointers = (uint16_t *)malloc(sizeof(uint16_t) * db->page_header.cell_count);
    for (size_t i = 0; i < db->page_header.cell_count; i++)
    {
        uint8_t buffer[2];
        fread(buffer, sizeof(uint8_t), 2, db->file);
        cell_pointers[i] = (uint16_t)from_be_bytes(buffer, 2);
    }

    size_t cell_count = db->page_header.cell_count;
    *names = (char **)malloc(sizeof(char *) * cell_count);
    for (size_t i = 0; i < cell_count; i++)
    {
        uint16_t cell_pointer = cell_pointers[i];

        uint8_t *cell_payload;
        size_t cell_payload_size;
        if (read_cell_payload(db->file, cell_pointer, db->page_header.type, usable_page_size, &cell_payload,
                              &cell_payload_size) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read cell payload\n");
            return 1;
        }

        RecordHeader record_header;
        if (read_record_header(cell_payload, cell_payload_size, &record_header) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read record header\n");
            return 1;
        }

        ColumnData *record_data = (ColumnData *)malloc(sizeof(ColumnData) * record_header.count);
        if (read_record_data(cell_payload, &record_header, record_data) != EXIT_SUCCESS)
        {
            free(record_data);
            fprintf(stderr, "Failed to read record data\n");
            return 1;
        }

        const size_t tbl_name_col = 2;
        char *table_name = record_data[tbl_name_col].value.text;

        char *pos = strstr(table_name, "sqlite_");
        if (pos && (pos - table_name) == 0)
        {
            free(record_data);
            continue;
        }

        char *return_name = (char *)malloc(sizeof(char) * (strlen(table_name) + 1));
        strcpy(return_name, table_name);

        (*names)[(*count)++] = return_name;
    }

    return EXIT_SUCCESS;
}