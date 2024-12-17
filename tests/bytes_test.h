#ifndef BYTES_TEST_H
#define BYTES_TEST_H

#include <munit.h>

MunitResult bytes_test_to_be_bytes(const MunitParameter params[], void *data);
MunitResult bytes_test_to_signed_be_bytes_positive(const MunitParameter params[], void *data);
MunitResult bytes_test_to_signed_be_bytes_negative(const MunitParameter params[], void *data);
MunitResult bytes_test_from_be_bytes(const MunitParameter params[], void *data);
MunitResult bytes_test_from_signed_be_bytes_positive(const MunitParameter params[], void *data);
MunitResult bytes_test_from_signed_be_bytes_negative(const MunitParameter params[], void *data);

#endif
