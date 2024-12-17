#include "cc-sqlite/record.h"
#include "cc-sqlite/varint.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int read_record_header(const uint8_t *payload, const size_t payload_size, RecordHeader *header)
{
    size_t buf_position = 0;

    uint64_t header_length;
    size_t varint_len = read_varint(payload, &header_length);
    buf_position += varint_len;

    ColumnHeader *columns;
    size_t columns_count = 0;
    size_t columns_capacity = 10;

    columns = (ColumnHeader *)malloc(sizeof(ColumnHeader) * columns_capacity);

    while (buf_position < (size_t)header_length)
    {
        uint64_t value;
        varint_len = read_varint(&(payload[buf_position]), &value);
        buf_position += varint_len;

        if (columns_count == columns_capacity)
        {
            columns_capacity *= 2;
            columns = realloc(columns, sizeof(ColumnHeader) * columns_capacity);
        }

        ColumnHeader *cur = &columns[columns_count++];
        switch (value)
        {
        case 0:
            cur->data_type = COL_DATA_TYPE_NULL;
            cur->data_length = 0;
            break;
        case 1:
            cur->data_type = COL_DATA_TYPE_I8;
            cur->data_length = 1;
            break;
        case 2:
            cur->data_type = COL_DATA_TYPE_I16;
            cur->data_length = 2;
            break;
        case 3:
            cur->data_type = COL_DATA_TYPE_I24;
            cur->data_length = 3;
            break;
        case 4:
            cur->data_type = COL_DATA_TYPE_I32;
            cur->data_length = 4;
            break;
        case 5:
            cur->data_type = COL_DATA_TYPE_I48;
            cur->data_length = 6;
            break;
        case 6:
            cur->data_type = COL_DATA_TYPE_I64;
            cur->data_length = 8;
            break;
        case 7:
            cur->data_type = COL_DATA_TYPE_F64;
            cur->data_length = 8;
            break;
        case 8:
            cur->data_type = COL_DATA_TYPE_ZERO;
            cur->data_length = 0;
            break;
        case 9:
            cur->data_type = COL_DATA_TYPE_ONE;
            cur->data_length = 0;
            break;
        case 10:
            [[fallthrough]];
        case 11:
            cur->data_type = COL_DATA_TYPE_INTERNAL;
            cur->data_length = 0;
            break;
        default:
            if (value % 2 == 0)
            {
                cur->data_type = COL_DATA_TYPE_BLOB;
                cur->data_length = (value - 12) / 2;
            }
            else
            {
                cur->data_type = COL_DATA_TYPE_TEXT;
                cur->data_length = (value - 13) / 2;
            }
            break;
        }
    }

    columns = realloc(columns, sizeof(ColumnHeader) * columns_count);

    header->columns = columns;
    header->count = columns_count;
    header->length = header_length;

    return EXIT_SUCCESS;
}
