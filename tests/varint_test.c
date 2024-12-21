#include "varint_test.h"

#include <stdint.h>

#include "cc-sqlite/varint.h"

#define VARINT_MAX_BYTES (9)

MunitResult varint_test_readwrite_varint(const MunitParameter params[], void *data)
{
    const char *value_param = munit_parameters_get(params, "varint_value");
    int64_t value = strtol(value_param, NULL, 16);

    uint8_t buffer[VARINT_MAX_BYTES];
    memset(buffer, 0, VARINT_MAX_BYTES);

    size_t bytes_written = write_varint(buffer, value);

    int64_t actual;

    size_t bytes_read = read_varint(buffer, &actual);

    munit_assert_uint64(actual, ==, value);
    munit_assert_size(bytes_read, ==, bytes_written);

    return MUNIT_OK;
}

MunitResult varint_test_read_varint_one_byte(const MunitParameter params[], void *data)
{
    uint8_t buffer[1] = {0x47};
    int64_t expected = 0x47;

    int64_t actual;
    size_t bytes_read = read_varint(buffer, &actual);

    munit_assert_int64(actual, ==, expected);
    munit_assert_size(bytes_read, ==, 1);

    return MUNIT_OK;
}

MunitResult varint_test_read_varint_two_bytes(const MunitParameter params[], void *data)
{
    uint8_t buffer[2] = {0x81, 0x47};
    int64_t expected = 0xC7;

    int64_t actual;
    size_t bytes_read = read_varint(buffer, &actual);

    munit_assert_int64(actual, ==, expected);
    munit_assert_size(bytes_read, ==, 2);

    return MUNIT_OK;
}

MunitResult varint_test_read_varint_nine_bytes(const MunitParameter params[], void *data)
{
    uint8_t buffer[9] = {0xBF, 0xDB, 0xEB, 0xF4, 0xF9, 0xFC, 0xDE, 0x9F, 0x0F};
    int64_t expected = 0x7F6F5F4F3F2F1F0F;

    int64_t actual;
    size_t bytes_read = read_varint(buffer, &actual);

    munit_assert_int64(actual, ==, expected);
    munit_assert_size(bytes_read, ==, 9);

    return MUNIT_OK;
}

MunitResult varint_test_write_varint_one_byte(const MunitParameter params[], void *data)
{
    uint8_t expected[1] = {0x47};
    int64_t value = 0x47;

    uint8_t actual[1] = {0};
    size_t bytes_written = write_varint(actual, value);

    munit_assert_memory_equal(1, actual, expected);
    munit_assert_size(bytes_written, ==, 1);

    return MUNIT_OK;
}

MunitResult varint_test_write_varint_two_bytes(const MunitParameter params[], void *data)
{
    uint8_t expected[2] = {0x81, 0x47};
    int64_t value = 0xC7;

    uint8_t actual[2] = {0};
    size_t bytes_written = write_varint(actual, value);

    munit_assert_memory_equal(2, actual, expected);
    munit_assert_size(bytes_written, ==, 2);

    return MUNIT_OK;
}

MunitResult varint_test_write_varint_nine_bytes(const MunitParameter params[], void *data)
{
    uint8_t expected[9] = {0xBF, 0xDB, 0xEB, 0xF4, 0xF9, 0xFC, 0xDE, 0x9F, 0x0F};
    int64_t value = 0x7F6F5F4F3F2F1F0F;

    uint8_t actual[9] = {0};
    size_t bytes_written = write_varint(actual, value);

    munit_assert_memory_equal(9, actual, expected);
    munit_assert_size(bytes_written, ==, 9);

    return MUNIT_OK;
}
