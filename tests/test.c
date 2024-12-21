#include <munit.h>

#include "bytes_test.h"
#include "file_header_test.h"
#include "varint_test.h"

static char *varint_value_params[] = {"0x0",
                                      "0x100",
                                      "0x10000",
                                      "0x1000000",
                                      "0x100000000",
                                      "0x10000000000",
                                      "0x1000000000000",
                                      "0x100000000000000",
                                      "0x7FFFFFFFFFFFFFFF",
                                      "-0x8000000000000000",
                                      NULL};

static MunitParameterEnum varint_test_params[] = {{"varint_value", varint_value_params}, {NULL, NULL}};

static MunitTest all_tests[] = {
    {(char *)"/varint/readwrite_varint", varint_test_readwrite_varint, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     varint_test_params},
    {(char *)"/varint/read_varint/1_byte", varint_test_read_varint_one_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/varint/read_varint/2_bytes", varint_test_read_varint_two_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {(char *)"/varint/read_varint/9_bytes", varint_test_read_varint_nine_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {(char *)"/varint/write_varint/1_byte", varint_test_write_varint_one_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {(char *)"/varint/write_varint/2_bytes", varint_test_write_varint_two_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {(char *)"/varint/write_varint/9_bytes", varint_test_write_varint_nine_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {(char *)"/bytes/to_be_bytes", bytes_test_to_be_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/bytes/to_signed_be_bytes/positive", bytes_test_to_signed_be_bytes_positive, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/bytes/to_signed_be_bytes/negative", bytes_test_to_signed_be_bytes_negative, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/bytes/from_be_bytes", bytes_test_from_be_bytes, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/bytes/from_signed_be_bytes/positive", bytes_test_from_signed_be_bytes_positive, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/bytes/from_signed_be_bytes/negative", bytes_test_from_signed_be_bytes_negative, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char *)"/file_header/file_header_read", file_header_test_file_header_read, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite test_suite = {(char *)"", all_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

#include <stdlib.h>

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    return munit_suite_main(&test_suite, (void *)"µnit", argc, argv);
}
