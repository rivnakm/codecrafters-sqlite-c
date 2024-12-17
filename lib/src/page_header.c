#include "cc-sqlite/page_header.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cc-sqlite/file_header.h"

int page_header_read(FILE *file, const uint16_t page_size, const size_t page, PageHeader *header)
{
    size_t page_offset = page_size * page;
    size_t start = page_offset + FILE_HEADER_SIZE;

    int err = fseek(file, start, SEEK_SET);
    if (err != 0)
    {
        return err;
    }

    uint8_t buffer[4];
    // B-Tree page type
    size_t bytes = fread(buffer, sizeof(uint8_t), 1, file);
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
    bytes = fread(buffer, sizeof(uint8_t), 2, file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->freeblock_offset = (buffer[0] << 8) | buffer[1];

    // Number of cells on the page
    bytes = fread(buffer, sizeof(uint8_t), 2, file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->cell_count = (buffer[0] << 8) | buffer[1];

    // Start of the cell content area. A zero value is interpreted as 65536
    bytes = fread(buffer, sizeof(uint8_t), 2, file);
    if (bytes < 2)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->cell_content_area_start = (buffer[0] << 8) | buffer[1];

    // Number of fragmented free bytes in the content area
    bytes = fread(buffer, sizeof(uint8_t), 1, file);
    if (bytes < 1)
    {
        fprintf(stderr, "EOF or error while reading page header");
        return EXIT_FAILURE;
    }

    header->frag_bytes_count = buffer[0];

    if (header->type == PAGE_TYPE_INTERIOR_INDEX || header->type == PAGE_TYPE_INTERIOR_TABLE)
    {
        // Rightmost pointer
        bytes = fread(buffer, sizeof(uint8_t), 4, file);
        if (bytes < 4)
        {
            fprintf(stderr, "EOF or error while reading page header");
            return EXIT_FAILURE;
        }

        header->end = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    }

    return EXIT_SUCCESS;
}
