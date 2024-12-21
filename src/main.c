#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc-sqlite/bytes.h"
#include "cc-sqlite/cell.h"
#include "cc-sqlite/file_header.h"
#include "cc-sqlite/page_header.h"
#include "cc-sqlite/record.h"

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

    FileHeader file_header;
    if (file_header_read(database_file, &file_header) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to read file header\n");
        return 1;
    }

    PageHeader page_header;
    if (page_header_read(database_file, file_header.page_size, 0, &page_header) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to read page header\n");
        return 1;
    }

    if (strcmp(command, ".dbinfo") == 0)
    {
        printf("database page size: %u\n", file_header.page_size);
        printf("number of tables: %u\n", page_header.cell_count);
    }
    else if (strcmp(command, ".tables") == 0)
    {
        uint16_t usable_page_size =
            file_header.page_size - file_header.reserved - 100; // first page has the db file header

        // cell pointers come right after the page header
        // file header is 100 bytes on page 1 only, otherwise 0
        size_t cell_pointers_offset = 100;
        if (page_header.type == PAGE_TYPE_INTERIOR_TABLE || page_header.type == PAGE_TYPE_INTERIOR_INDEX)
        {
            cell_pointers_offset += 12;
        }
        else
        {
            cell_pointers_offset += 8;
        }

        fseek(database_file, cell_pointers_offset, SEEK_SET);
        uint16_t *cell_pointers = (uint16_t *)malloc(sizeof(uint16_t) * page_header.cell_count);
        for (size_t i = 0; i < page_header.cell_count; i++)
        {
            uint8_t buffer[2];
            fread(buffer, sizeof(uint8_t), 2, database_file);
            cell_pointers[i] = (uint16_t)from_be_bytes(buffer, 2);
        }

        for (size_t i = 0; i < page_header.cell_count; i++)
        {
            uint16_t cell_pointer = cell_pointers[i];

            uint8_t *cell_payload;
            size_t cell_payload_size;
            if (read_cell_payload(database_file, cell_pointer, page_header.type, usable_page_size, &cell_payload,
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
                fprintf(stderr, "Failed to read record data\n");
                return 1;
            }

            const size_t tbl_name_col = 2;
            if (i < page_header.cell_count - 1)
            {
                printf("%s ", record_data[tbl_name_col].value.text);
            }
            else
            {
                printf("%s\n", record_data[tbl_name_col].value.text);
            }
        }

        fclose(database_file);
    }
    else
    {
        fprintf(stderr, "Unknown command %s\n", command);
        return 1;
    }

    return 0;
}
