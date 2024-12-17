#ifndef PAGE_HEADER_H
#define PAGE_HEADER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum
{
    PAGE_TYPE_INTERIOR_TABLE,
    PAGE_TYPE_INTERIOR_INDEX,
    PAGE_TYPE_LEAF_TABLE,
    PAGE_TYPE_LEAF_INDEX,
} PageType;

typedef struct
{
    PageType type;

    uint16_t freeblock_offset;
    uint16_t cell_count;
    uint16_t cell_content_area_start;
    uint8_t frag_bytes_count;

    // Interior pages only
    uint32_t end;

} PageHeader;

int page_header_read(FILE *file, const uint16_t page_size, const size_t page, PageHeader *header);

#endif
