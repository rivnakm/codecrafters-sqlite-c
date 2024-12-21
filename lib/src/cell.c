#include "cc-sqlite/cell.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cc-sqlite/bytes.h"
#include "cc-sqlite/page_header.h"
#include "cc-sqlite/varint.h"

typedef struct
{
    size_t length;
    uint32_t left;
    uint64_t payload_size;
    int64_t key;
    uint32_t overflow_page;
} CellHeader;

/**
 * Read cell header from file
 *
 * @param file File pointer to read
 * @param cell_pointer Cell start position
 * @param page_type Page type
 * @param usable_page_size Usable page size, page size less reserved space
 * @param header [out] Cell header
 * @return Status code
 */
int cell_header_read(FILE *file, const uint16_t cell_pointer, const PageType page_type, const uint16_t usable_page_size,
                     CellHeader *header);

int read_leaf_table_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size);
int read_interior_table_cell_header(FILE *file, CellHeader *header);
int read_leaf_index_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size);
int read_interior_index_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size);

uint16_t non_overflowing_payload_len(const uint16_t usable_page_size, const uint16_t available_payload_space,
                                     const uint64_t payload_size);

// -----------
// Definitions
// -----------

int read_cell_payload(FILE *file, const uint16_t cell_pointer, const PageType page_type,
                      const uint16_t usable_page_size, uint8_t **data, size_t *size)
{
    CellHeader header;
    int err = cell_header_read(file, cell_pointer, page_type, usable_page_size, &header);
    if (err != 0)
    {
        return err;
    }
    *size = header.payload_size;

    err = fseek(file, cell_pointer, SEEK_SET);
    if (err != 0)
    {
        return err;
    }

    *data = (uint8_t *)malloc(sizeof(uint8_t) * header.payload_size);
    if (*data == NULL)
    {
        return EXIT_FAILURE;
    }

    if (header.overflow_page == 0)
    {
        err = fseek(file, header.length, SEEK_CUR);
        if (err != 0)
        {
            return err;
        }

        fread(*data, sizeof(uint8_t), header.payload_size, file);
    }
    else
    {
        fprintf(stderr, "Cell payload has overflow page, which is unimplemented\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int cell_header_read(FILE *file, const uint16_t cell_pointer, const PageType page_type, const uint16_t usable_page_size,
                     CellHeader *header)
{
    int err = fseek(file, cell_pointer, SEEK_SET);
    if (err != 0)
    {
        return err;
    }

    switch (page_type)
    {
    case PAGE_TYPE_INTERIOR_TABLE:
        return read_interior_table_cell_header(file, header);
    case PAGE_TYPE_INTERIOR_INDEX:
        return read_interior_index_cell_header(file, header, usable_page_size);
    case PAGE_TYPE_LEAF_TABLE:
        return read_leaf_table_cell_header(file, header, usable_page_size);
    case PAGE_TYPE_LEAF_INDEX:
        return read_leaf_index_cell_header(file, header, usable_page_size);
    }
}

/// The seemingly arbitrary formulas below can be referenced here
/// https://www.sqlite.org/fileformat.html#pages
int read_leaf_table_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size)
{
    const size_t bufsize = 18;
    uint8_t buf[bufsize];
    fread(buf, sizeof(uint8_t), bufsize, file);
    // TODO: handle error

    int64_t varint_val;
    size_t varint_bytes = read_varint(buf, &varint_val);
    header->length = varint_bytes;
    header->payload_size = (uint64_t)varint_val;

    varint_bytes = read_varint(&(buf[varint_bytes]), &header->key);
    header->length += varint_bytes;

    // seek back to start of the cell
    fseek(file, -bufsize, SEEK_CUR);

    uint16_t available_payload_space = usable_page_size - 35;

    if (header->payload_size <= available_payload_space)
    {
        header->overflow_page = 0;
    }
    else
    {
        uint16_t payload_on_page =
            non_overflowing_payload_len(usable_page_size, available_payload_space, header->payload_size);

        fseek(file, header->length + payload_on_page, SEEK_CUR);

        fread(buf, sizeof(uint8_t), 4, file);
        // TODO: handle error

        header->overflow_page = from_be_bytes(buf, 4);
    }

    return EXIT_SUCCESS;
}

int read_interior_table_cell_header(FILE *file, CellHeader *header)
{
    uint8_t buf[9];

    header->length = 4;
    fread(buf, sizeof(uint8_t), 4, file);
    // TODO: handle error

    header->left = from_be_bytes(buf, 4);

    header->length += read_varint(buf, &header->key);

    return EXIT_SUCCESS;
}

int read_leaf_index_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size)
{
    const size_t bufsize = 9;
    uint8_t buf[bufsize];
    fread(buf, sizeof(uint8_t), bufsize, file);
    // TODO: handle error

    int64_t varint_val;
    header->length = read_varint(buf, &varint_val);
    header->payload_size = (uint64_t)varint_val;

    // seek back to start of the cell
    fseek(file, -bufsize, SEEK_CUR);

    uint16_t available_payload_space = usable_page_size - 35;

    if (header->payload_size <= available_payload_space)
    {
        header->overflow_page = 0;
    }
    else
    {
        uint16_t payload_on_page =
            non_overflowing_payload_len(usable_page_size, available_payload_space, header->payload_size);

        fseek(file, header->length + payload_on_page, SEEK_CUR);

        fread(buf, sizeof(uint8_t), 4, file);
        // TODO: handle error

        header->overflow_page = from_be_bytes(buf, 4);
    }

    return EXIT_SUCCESS;
}

int read_interior_index_cell_header(FILE *file, CellHeader *header, const uint16_t usable_page_size)
{
    const size_t bufsize = 13;
    uint8_t buf[bufsize];
    fread(buf, sizeof(uint8_t), bufsize, file);
    // TODO: handle error

    header->left = from_be_bytes(buf, 4);
    header->length = 4;

    int64_t varint_val;
    size_t varint_bytes = read_varint(&(buf[4]), &varint_val);
    header->length += varint_bytes;
    header->payload_size = (uint64_t)varint_val;

    // seek back to start of the cell
    fseek(file, -bufsize, SEEK_CUR);

    uint16_t available_payload_space = usable_page_size - 35;

    if (header->payload_size <= available_payload_space)
    {
        header->overflow_page = 0;
    }
    else
    {
        uint16_t payload_on_page =
            non_overflowing_payload_len(usable_page_size, available_payload_space, header->payload_size);

        fseek(file, header->length + payload_on_page, SEEK_CUR);

        fread(buf, sizeof(uint8_t), 4, file);
        // TODO: handle error

        header->overflow_page = from_be_bytes(buf, 4);
    }

    return EXIT_SUCCESS;
}

uint16_t non_overflowing_payload_len(const uint16_t usable_page_size, const uint16_t available_payload_space,
                                     const uint64_t payload_size)
{
    if (payload_size <= available_payload_space)
    {
        return (uint16_t)payload_size;
    }
    else
    {
        uint16_t minimum_payload = ((usable_page_size - 12) * 32 / 255) - 23;
        uint16_t payload_first_part = minimum_payload + ((payload_size - minimum_payload) % (usable_page_size - 4));

        if (payload_first_part <= available_payload_space)
        {
            return payload_first_part;
        }
        else
        {
            return minimum_payload;
        }
    }
}
