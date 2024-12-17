#include "bytes_test.h"
#include "cc-sqlite/bytes.h"
#include <stdint.h>

MunitResult bytes_test_to_be_bytes(const MunitParameter params[], void *data)
{
    uint64_t value = 0x123456;
    uint8_t expected[3] = {0x12, 0x34, 0x56};

    uint8_t buffer[3] = {0};

    to_be_bytes(buffer, value, 3);

    munit_assert_memory_equal(3, buffer, expected);

    return MUNIT_OK;
}

MunitResult bytes_test_to_signed_be_bytes_positive(const MunitParameter params[], void *data)
{
    int64_t value = 0x5960;
    uint8_t expected[2] = {0x59, 0x60};

    uint8_t buffer[2] = {0};

    to_signed_be_bytes(buffer, value, 2);

    return MUNIT_OK;
}

MunitResult bytes_test_to_signed_be_bytes_negative(const MunitParameter params[], void *data)
{
    int64_t value = -0x5960;
    uint8_t expected[2] = {0xA6, 0xA0};

    uint8_t buffer[2] = {0};

    to_signed_be_bytes(buffer, value, 2);

    return MUNIT_OK;
}

MunitResult bytes_test_from_be_bytes(const MunitParameter params[], void *data)
{
    uint64_t expected = 0x123456;
    uint8_t buffer[3] = {0x12, 0x34, 0x56};

    uint64_t value = from_be_bytes(buffer, 3);

    munit_assert_uint64(value, ==, expected);

    return MUNIT_OK;
}

MunitResult bytes_test_from_signed_be_bytes_positive(const MunitParameter params[], void *data)
{
    int64_t expected = 0x5960;
    uint8_t buffer[2] = {0x59, 0x60};

    int64_t value = from_signed_be_bytes(buffer, 2);

    munit_assert_int64(value, ==, expected);

    return MUNIT_OK;
}

MunitResult bytes_test_from_signed_be_bytes_negative(const MunitParameter params[], void *data)
{
    int64_t expected = -0x5960;
    uint8_t buffer[2] = {0xA6, 0xA0};

    int64_t value = from_signed_be_bytes(buffer, 2);

    munit_assert_int64(value, ==, expected);

    return MUNIT_OK;
}
