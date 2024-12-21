#ifndef VARINT_TEST_H
#define VARINT_TEST_H

#include <munit.h>

MunitResult varint_test_readwrite_varint(const MunitParameter params[], void *data);
MunitResult varint_test_read_varint_one_byte(const MunitParameter params[], void *data);
MunitResult varint_test_read_varint_two_bytes(const MunitParameter params[], void *data);
MunitResult varint_test_read_varint_nine_bytes(const MunitParameter params[], void *data);
MunitResult varint_test_write_varint_one_byte(const MunitParameter params[], void *data);
MunitResult varint_test_write_varint_two_bytes(const MunitParameter params[], void *data);
MunitResult varint_test_write_varint_nine_bytes(const MunitParameter params[], void *data);

#endif
