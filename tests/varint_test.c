#include "varint_test.h"

#include <stdint.h>

#include "cc-sqlite/varint.h"

#define VARINT_MAX_BYTES (9)

MunitResult varint_test_readwrite_varint(const MunitParameter params[], void *data)
{
    const char *value_param = munit_parameters_get(params, "varint_value");
    uint64_t value = strtoul(value_param, NULL, 10);

    uint8_t buffer[VARINT_MAX_BYTES];
    memset(buffer, 0, VARINT_MAX_BYTES);

    size_t bytes_written = write_varint(buffer, value);

    uint64_t actual;

    size_t bytes_read = read_varint(buffer, &actual);

    munit_assert_uint64(actual, ==, value);
    munit_assert_size(bytes_read, ==, bytes_written);

    return MUNIT_OK;
}
