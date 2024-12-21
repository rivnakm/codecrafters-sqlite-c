#include "cc-sqlite/record.h"
#include "cc-sqlite/bytes.h"
#include "cc-sqlite/varint.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_record_header(const uint8_t *payload, const size_t payload_size, RecordHeader *header)
{
    size_t buf_position = 0;

    int64_t header_length;
    size_t varint_len = read_varint(payload, &header_length);
    buf_position += varint_len;

    ColumnHeader *columns;
    size_t columns_count = 0;
    size_t columns_capacity = 10;

    columns = (ColumnHeader *)malloc(sizeof(ColumnHeader) * columns_capacity);

    while (buf_position < (size_t)header_length)
    {
        int64_t value;
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

int read_record_data(const uint8_t *payload, const RecordHeader *header, ColumnData *data)
{
    size_t position = header->length;
    for (size_t i = 0; i < header->count; i++)
    {
        ColumnHeader *current = &header->columns[i];
        ColumnValue value;
        switch (current->data_type)
        {
        case COL_DATA_TYPE_NULL:
            break;
        case COL_DATA_TYPE_I8:
            [[fallthrough]];
        case COL_DATA_TYPE_I16:
            [[fallthrough]];
        case COL_DATA_TYPE_I24:
            [[fallthrough]];
        case COL_DATA_TYPE_I32:
            [[fallthrough]];
        case COL_DATA_TYPE_I48:
            [[fallthrough]];
        case COL_DATA_TYPE_I64:
            value.integer = from_signed_be_bytes(payload + position, current->data_length);
            break;
        case COL_DATA_TYPE_F64:
            // TODO: parse float from bytes
            fprintf(stderr, "Unimplemented: float record value\n");
            exit(-1);
        case COL_DATA_TYPE_ZERO:
            value.integer = 0;
            break;
        case COL_DATA_TYPE_ONE:
            value.integer = 1;
            break;
        case COL_DATA_TYPE_INTERNAL:
            fprintf(stderr, "Unsupported: internal data type\n");
            exit(-1);
        case COL_DATA_TYPE_BLOB:
            value.blob = (uint8_t *)malloc(sizeof(uint8_t) * current->data_length);
            memcpy(value.blob, payload + position, current->data_length);
            break;
        case COL_DATA_TYPE_TEXT:
            value.text = (char *)malloc(sizeof(char) * (current->data_length + 1));
            memcpy(value.text, payload + position, current->data_length);
            value.text[current->data_length] = '\0';
            break;
        }

        data[i] = (ColumnData){.data_type = current->data_type, .data_length = current->data_length, .value = value};

        position += current->data_length;
    }

    return EXIT_SUCCESS;
}
