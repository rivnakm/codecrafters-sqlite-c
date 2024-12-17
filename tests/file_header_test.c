#include "file_header_test.h"
#include "cc-sqlite/file_header.h"

#include <stdio.h>
#include <stdlib.h>

MunitResult file_header_test_file_header_read(const MunitParameter params[], void *data)
{
    FILE *file = fopen(EXAMPLES_DIR "/empty.db", "rb");
    if (!file)
    {
        return MUNIT_FAIL;
    }

    FileHeader header;
    int err = file_header_read(file, &header);

    munit_assert_int(err, ==, EXIT_SUCCESS);

    munit_assert_uint16(header.page_size, ==, 4096);
    munit_assert_uint8(header.reserved, ==, 0);

    return MUNIT_OK;
}
