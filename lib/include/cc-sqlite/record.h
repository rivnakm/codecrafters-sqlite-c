#ifndef RECORD_H
#define RECORD_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    COL_DATA_TYPE_NULL,
    COL_DATA_TYPE_I8,
    COL_DATA_TYPE_I16,
    COL_DATA_TYPE_I24,
    COL_DATA_TYPE_I32,
    COL_DATA_TYPE_I48,
    COL_DATA_TYPE_I64,
    COL_DATA_TYPE_F64,
    COL_DATA_TYPE_ZERO,
    COL_DATA_TYPE_ONE,
    COL_DATA_TYPE_INTERNAL,
    COL_DATA_TYPE_BLOB,
    COL_DATA_TYPE_TEXT,
} ColumnDataType;

typedef union {
    int64_t integer;
    double floating_point;
    uint8_t *blob;
    char *text;
} ColumnData;

typedef struct
{
    ColumnDataType data_type;
    size_t data_length;
} ColumnHeader;

typedef struct
{
    ColumnHeader *columns;
    size_t count;
    size_t length;
} RecordHeader;

int read_record_header(const uint8_t *payload, const size_t payload_size, RecordHeader *header);

#endif
