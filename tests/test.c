#include <munit.h>

#include "bytes_test.h"
#include "file_header_test.h"
#include "varint_test.h"

static char *varint_value_params[] = {"0",
                                      "240",
                                      "2287",
                                      "67823",
                                      "16777215",
                                      "4294967295",
                                      "1099511627775",
                                      "281474976710655",
                                      "72057594037927935",
                                      "72057594037927936",
                                      NULL};

static MunitParameterEnum varint_test_params[] = {{"varint_value", varint_value_params}, {NULL, NULL}};

static MunitTest all_tests[] = {
    {(char *)"/varint/readwrite_varint", varint_test_readwrite_varint, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     varint_test_params},
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
