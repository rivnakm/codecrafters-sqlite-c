#include "cc_sqlite/database.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc_sqlite/bytes.h"
#include "cc_sqlite/cell.h"
#include "cc_sqlite/page_header.h"
#include "cc_sqlite/record.h"
#include "cc_sqlite/schema/row.h"

int db_open(FILE *file, Database *db)
{
    db->file = file;
    if (file_header_read(file, &db->file_header) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to read file header\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int db_get_table_records(const Database *db, const PageHeader *page_header, Record **records, size_t *count)
{
    *count = 0;

    uint16_t usable_page_size = db->file_header.page_size - db->file_header.reserved -
                                (page_header->has_file_header ? 100 : 0); // first page has the db file header

    // cell pointers come right after the page header
    // file header is 100 bytes on page 1 only, otherwise 0
    size_t cell_pointers_offset = 100;
    if (page_header->type == PAGE_TYPE_INTERIOR_TABLE || page_header->type == PAGE_TYPE_INTERIOR_INDEX)
    {
        cell_pointers_offset += 12;
    }
    else
    {
        cell_pointers_offset += 8;
    }
    fseek(db->file, cell_pointers_offset, SEEK_SET);
    uint16_t *cell_pointers = (uint16_t *)malloc(sizeof(uint16_t) * page_header->cell_count);
    for (size_t i = 0; i < page_header->cell_count; i++)
    {
        uint8_t buffer[2];
        fread(buffer, sizeof(uint8_t), 2, db->file);
        cell_pointers[i] = (uint16_t)from_be_bytes(buffer, 2);
    }

    size_t cell_count = page_header->cell_count;
    *records = (Record *)malloc(sizeof(Record) * cell_count);
    for (size_t i = 0; i < cell_count; i++)
    {
        uint16_t cell_pointer = cell_pointers[i];

        uint8_t *cell_payload;
        size_t cell_payload_size;
        if (read_cell_payload(db->file, cell_pointer, page_header->type, usable_page_size, &cell_payload,
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

        Column *columns = (Column *)malloc(sizeof(Column) * record_header.count);
        if (read_record_data(cell_payload, &record_header, columns) != EXIT_SUCCESS)
        {
            free(columns);
            fprintf(stderr, "Failed to read record data\n");
            return 1;
        }

        (*records)[(*count)++] = (Record){.header = record_header, .columns = columns};
    }
    return EXIT_SUCCESS;
}

int db_get_schema_table_rows(const Database *db, SchemaRow *rows[], size_t *count)
{
    *count = 0;
    PageHeader page_header;
    int err = db_get_page_header(db, 0, &page_header);
    if (err != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    Record *records = NULL;
    size_t records_count;
    err = db_get_table_records(db, &page_header, &records, &records_count);
    if (err != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    *rows = (SchemaRow *)malloc(sizeof(SchemaRow) * records_count);
    for (size_t i = 0; i < records_count; i++)
    {
        Column *columns = records[i].columns;

        const size_t type_col = 0;
        const size_t name_col = 1;
        const size_t tbl_name_col = 2;
        const size_t rootpage_col = 3;

        char *type_str = columns[type_col].data->value.text;
        char *row_name = strdup(columns[name_col].data->value.text);
        char *row_table_name = strdup(columns[tbl_name_col].data->value.text);

        SchemaRowType row_type = str_to_row_type(type_str);

        (*rows)[(*count)++] = (SchemaRow){.type = row_type,
                                          .name = row_name,
                                          .table_name = row_table_name,
                                          .rootpage = columns[rootpage_col].data->value.integer};
    }
    for (size_t i = 0; i < records_count; i++)
    {
        Record *record = &records[i];
        record_data_free(record->columns[i].data);
        record->columns[i].data = NULL;
    }
    free(records);

    return EXIT_SUCCESS;
}

int db_get_table_names(const Database *db, char **names[], size_t *count)
{
    *count = 0;

    SchemaRow *rows;
    size_t rows_count;
    int err = db_get_schema_table_rows(db, &rows, &rows_count);
    if (err != EXIT_SUCCESS)
    {
        return err;
    }

    *names = (char **)malloc(sizeof(char *) * *count);
    for (size_t i = 0; i < rows_count; i++)
    {
        char *pos = strstr(rows[i].table_name, "sqlite_");
        if (pos && (pos - rows[i].table_name) == 0)
        {
            schema_row_free(&rows[i]);
            continue;
        }

        (*names)[(*count)++] = strdup(rows[i].table_name);
        schema_row_free(&rows[i]);
    }

    free(rows);

    return EXIT_SUCCESS;
}

int db_get_table_by_name(const Database *db, const char *table_name, SchemaRow *row)
{
    bool found = false;

    SchemaRow *rows;
    size_t rows_count;
    int err = db_get_schema_table_rows(db, &rows, &rows_count);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to get schema table rows\n");
        return err;
    }

    for (size_t i = 0; i < rows_count; i++)
    {
        if (strcmp(rows[i].table_name, table_name) != 0)
        {
            schema_row_free(&rows[i]);
            continue;
        }

        found = true;
        *row = rows[i];
    }

    free(rows);

    if (!found)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int db_get_page_header(const Database *db, const size_t page, PageHeader *header)
{
    header->has_file_header = page == 0;

    size_t page_offset = db->file_header.page_size * page;
    size_t start = page_offset + (header->has_file_header ? FILE_HEADER_SIZE : 0);

    int err = fseek(db->file, start, SEEK_SET);
    if (err != 0)
    {
        return err;
    }

    uint8_t buffer[4];
    // B-Tree page type
    size_t bytes = fread(buffer, sizeof(uint8_t), 1, db->file);
    if (bytes < 1)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    switch (buffer[0])
    {
    case 0x02:
        header->type = PAGE_TYPE_INTERIOR_INDEX;
        break;
    case 0x05:
        header->type = PAGE_TYPE_INTERIOR_TABLE;
        break;
    case 0x0A:
        header->type = PAGE_TYPE_LEAF_INDEX;
        break;
    case 0x0D:
        header->type = PAGE_TYPE_LEAF_TABLE;
        break;
    default:
        fprintf(stderr, "Invalid page header type flag %02X\n", buffer[0]);
        return EXIT_FAILURE;
    }

    // Start of the freeblock on the page, or zero if there are none
    bytes = fread(buffer, sizeof(uint8_t), 2, db->file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->freeblock_offset = (buffer[0] << 8) | buffer[1];

    // Number of cells on the page
    bytes = fread(buffer, sizeof(uint8_t), 2, db->file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->cell_count = (buffer[0] << 8) | buffer[1];

    // Start of the cell content area. A zero value is interpreted as 65536
    bytes = fread(buffer, sizeof(uint8_t), 2, db->file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->cell_content_area_start = (buffer[0] << 8) | buffer[1];

    // Number of fragmented free bytes in the content area
    bytes = fread(buffer, sizeof(uint8_t), 1, db->file);
    if (bytes < 1)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->frag_bytes_count = buffer[0];

    if (header->type == PAGE_TYPE_INTERIOR_INDEX || header->type == PAGE_TYPE_INTERIOR_TABLE)
    {
        // Rightmost pointer
        bytes = fread(buffer, sizeof(uint8_t), 4, db->file);
        if (bytes < 4)
        {
            fprintf(stderr, "EOF or error while reading page header");
            return EXIT_FAILURE;
        }

        header->end = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    }

    return EXIT_SUCCESS;
}
